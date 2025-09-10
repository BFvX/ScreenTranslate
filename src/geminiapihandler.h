#ifndef GEMINIAPIHANDLER_H
#define GEMINIAPIHANDLER_H

#include <QObject>
#include <QString>
#include <QStringList>
#include <QPixmap>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkProxy>

class GeminiApiHandler : public QObject
{
    Q_OBJECT

public:
    explicit GeminiApiHandler(QObject *parent = nullptr);
    void translateImage(const QPixmap &image, const QStringList &apiKeys, const QString &prompt, const QString &modelName, const QList<QJsonObject> &history);
    void testApiConnection(const QString &apiKey);
    void setProxy(bool enabled, const QString &hostName = "127.0.0.1", quint16 port = 10808);

signals:
    void translationReady(const QString &translatedText);
    void errorOccurred(const QString &errorString);
    void apiTestSuccess(const QString &message);
    void apiTestFailed(const QString &errorString);

private:
    QNetworkAccessManager *networkManager;
};

#endif // GEMINIAPIHANDLER_H
