#include "geminiapihandler.h"
#include <QBuffer>
#include <QByteArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QUrlQuery>
#include <functional>

GeminiApiHandler::GeminiApiHandler(QObject *parent) : QObject(parent)
{
    networkManager = new QNetworkAccessManager(this);
}

void GeminiApiHandler::setProxy(bool enabled, const QString &hostName, quint16 port)
{
    if (enabled) {
        QNetworkProxy proxy;
        proxy.setType(QNetworkProxy::HttpProxy);
        proxy.setHostName(hostName);
        proxy.setPort(port);
        networkManager->setProxy(proxy);
    } else {
        networkManager->setProxy(QNetworkProxy::NoProxy);
    }
}

void GeminiApiHandler::testApiConnection(const QString &apiKey)
{
    QUrl url("https://generativelanguage.googleapis.com/v1beta/models");
    QUrlQuery query;
    query.addQueryItem("key", apiKey);
    url.setQuery(query);

    QNetworkRequest request(url);
    QNetworkReply *reply = networkManager->get(request);

    connect(reply, &QNetworkReply::finished, this, [this, reply]() {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();
            QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
            if (jsonDoc.object().contains("models")) {
                emit apiTestSuccess("Successfully fetched model list.");
            } else if (jsonDoc.object().contains("error")) {
                QString errorMsg = jsonDoc.object()["error"].toObject()["message"].toString();
                emit apiTestFailed("API Error: " + errorMsg);
            } else {
                emit apiTestFailed("Unknown API response format.");
            }
        } else {
            reply->readAll(); // Clear buffer on error
            emit apiTestFailed("Network Error: " + reply->errorString());
        }
        reply->deleteLater();
    });
}

void GeminiApiHandler::translateImage(const QPixmap &image, const QStringList &apiKeys, const QString &prompt, const QString &modelName, const QList<QJsonObject> &history)
{
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");
    QString base64Image = byteArray.toBase64();

    QJsonObject userTextPart, userImagePart, inlineData;
    userTextPart["text"] = prompt;
    inlineData["mime_type"] = "image/png";
    inlineData["data"] = base64Image;
    userImagePart["inline_data"] = inlineData;
    QJsonArray currentUserParts;
    currentUserParts.append(userTextPart);
    currentUserParts.append(userImagePart);
    QJsonObject currentUserContent;
    currentUserContent["role"] = "user";
    currentUserContent["parts"] = currentUserParts;

    QJsonArray contentsArray;
    for(const QJsonObject &item : history) {
        contentsArray.append(item);
    }
    contentsArray.append(currentUserContent);

    QJsonObject finalPayload;
    finalPayload["contents"] = contentsArray;
    QJsonDocument jsonDoc(finalPayload);
    QByteArray jsonData = jsonDoc.toJson();

    QString urlString = QString("https://generativelanguage.googleapis.com/v1beta/models/%1:generateContent").arg(modelName);

    auto lastError = std::make_shared<QString>();
    auto sendRequest = std::make_shared<std::function<void(int)>>();

    *sendRequest = [=, this, sendRequest, lastError](int index) {
        if (index >= apiKeys.size()) {
            emit errorOccurred(QStringLiteral("All API keys failed. Last error: %1").arg(*lastError));
            return;
        }

        QUrl url(urlString);
        QUrlQuery query;
        query.addQueryItem("key", apiKeys.at(index));
        url.setQuery(query);

        QNetworkRequest request(url);
        request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

        QNetworkReply *reply = networkManager->post(request, jsonData);

        connect(reply, &QNetworkReply::finished, this, [=, this, sendRequest, lastError, index]() {
            if (reply->error() == QNetworkReply::NoError) {
                QByteArray responseData = reply->readAll();
                QJsonDocument jsonDoc = QJsonDocument::fromJson(responseData);
                QJsonObject jsonObj = jsonDoc.object();

                if (jsonObj.contains("candidates")) {
                    QJsonArray candidates = jsonObj["candidates"].toArray();
                    if (!candidates.isEmpty()) {
                        QJsonObject firstCandidate = candidates[0].toObject();
                        QJsonObject content = firstCandidate["content"].toObject();
                        QJsonArray parts = content["parts"].toArray();
                        if (!parts.isEmpty()) {
                            QString translatedText = parts[0].toObject()["text"].toString();
                            emit translationReady(translatedText);
                        } else {
                            *lastError = QStringLiteral("API response format error: 'parts' array is empty.");
                            (*sendRequest)(index + 1);
                        }
                    } else {
                        *lastError = QStringLiteral("API response format error: 'candidates' array is empty.");
                        (*sendRequest)(index + 1);
                    }
                } else if (jsonObj.contains("error")) {
                    *lastError = jsonObj["error"].toObject()["message"].toString();
                    (*sendRequest)(index + 1);
                } else {
                    *lastError = QStringLiteral("API response format error: 'candidates' not found.");
                    (*sendRequest)(index + 1);
                }
            } else {
                reply->readAll();
                *lastError = reply->errorString();
                (*sendRequest)(index + 1);
            }
            reply->deleteLater();
        });
    };

    (*sendRequest)(0);
}
