#include "mainwindow.h"
#include "settingsdialog.h"
#include "geminiapihandler.h"
#include "screensnippet.h"

#include <QApplication>
#include <QFile>
#include <QTextStream>

#include <QPushButton>
#include <QTextEdit>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QMessageBox>
#include <QGuiApplication>
#include <QScreen>
#include <QJsonArray>
#include <QBuffer>
#include <QSettings>
#include <QMouseEvent>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), fixedRegion(QRect()), snippetWidget(nullptr), isMovingWindow(false)
{
    this->resize(800, 350);
    this->setMinimumSize(600, 250);
    this->setWindowFlags(Qt::FramelessWindowHint);
    // this->setAttribute(Qt::WA_TranslucentBackground);

    setupUI();

    // --- Logic & Connections ---
    apiHandler = new GeminiApiHandler(this);
    settingsDialog = new SettingsDialog(this);

    connect(setFixedRegionButton, &QPushButton::clicked, this, &MainWindow::onSetFixedRegionClicked);
    connect(translateFixedRegionButton, &QPushButton::clicked, this, &MainWindow::onTranslateFixedRegionClicked);
    connect(singleTranslateButton, &QPushButton::clicked, this, &MainWindow::onSingleTranslateClicked);
    connect(settingsButton, &QPushButton::clicked, this, &MainWindow::onSettingsClicked);
    connect(minimizeButton, &QPushButton::clicked, this, &MainWindow::showMinimized);
    connect(maximizeButton, &QPushButton::clicked, this, [this](){ this->isMaximized() ? this->showNormal() : this->showMaximized(); });
    connect(closeButton, &QPushButton::clicked, this, &MainWindow::close);

    connect(apiHandler, &GeminiApiHandler::translationReady, this, &MainWindow::onTranslationReady);
    connect(apiHandler, &GeminiApiHandler::errorOccurred, this, &MainWindow::onTranslationError);
    connect(settingsDialog, &SettingsDialog::apiTestRequested, this, &MainWindow::handleApiTestRequest);
    connect(apiHandler, &GeminiApiHandler::apiTestSuccess, this, &MainWindow::handleApiTestSuccess);
    connect(apiHandler, &GeminiApiHandler::apiTestFailed, this, &MainWindow::handleApiTestFailed);

    loadSettings();
}

void MainWindow::setupUI() {
    this->setObjectName("bgApp");

    centralWidgetFrame = new QFrame(this);
    centralWidgetFrame->setLayout(new QHBoxLayout());
    centralWidgetFrame->layout()->setContentsMargins(0, 0, 0, 0);
    centralWidgetFrame->layout()->setSpacing(0);
    this->setCentralWidget(centralWidgetFrame);

    leftMenuBg = new QFrame(this);
    leftMenuBg->setObjectName("leftMenuBg");
    leftMenuBg->setMinimumWidth(60);
    leftMenuBg->setMaximumWidth(60);
    QVBoxLayout* leftMenuLayout = new QVBoxLayout(leftMenuBg);
    leftMenuLayout->setContentsMargins(0,0,0,0);
    leftMenuLayout->setSpacing(0);
    centralWidgetFrame->layout()->addWidget(leftMenuBg);

    contentArea = new QFrame(this);
    QVBoxLayout* contentAreaLayout = new QVBoxLayout(contentArea);
    contentAreaLayout->setContentsMargins(0,0,0,0);
    contentAreaLayout->setSpacing(0);
    centralWidgetFrame->layout()->addWidget(contentArea);

    contentTopBg = new QFrame(this);
    contentTopBg->setObjectName("contentTopBg");
    contentTopBg->setMinimumHeight(60);
    contentTopBg->setMaximumHeight(60);
    QHBoxLayout* topBarLayout = new QHBoxLayout(contentTopBg);
    titleLabel = new QLabel("Screen Translator");
    titleLabel->setStyleSheet("font: 63 12pt \"Segoe UI Semibold\";");
    rightButtons = new QFrame();
    rightButtons->setObjectName("rightButtons");
    rightButtons->setLayout(new QHBoxLayout());
    rightButtons->layout()->setContentsMargins(0,0,0,0);
    rightButtons->layout()->setSpacing(5);
    minimizeButton = new QPushButton("-");
    maximizeButton = new QPushButton("[]");
    closeButton = new QPushButton("X");
    minimizeButton->setFixedSize(28,28);
    maximizeButton->setFixedSize(28,28);
    closeButton->setFixedSize(28,28);
    rightButtons->layout()->addWidget(minimizeButton);
    rightButtons->layout()->addWidget(maximizeButton);
    rightButtons->layout()->addWidget(closeButton);
    topBarLayout->addWidget(titleLabel);
    topBarLayout->addStretch();
    topBarLayout->addWidget(rightButtons);
    contentAreaLayout->addWidget(contentTopBg);

    pagesContainer = new QFrame(this);
    pagesContainer->setObjectName("pagesContainer");
    page1Layout = new QVBoxLayout(pagesContainer);
    contentAreaLayout->addWidget(pagesContainer);

    contentBottom = new QFrame(this);
    contentBottom->setObjectName("bottomBar");
    contentBottom->setMinimumHeight(30);
    contentBottom->setMaximumHeight(30);
    QHBoxLayout* bottomBarLayout = new QHBoxLayout(contentBottom);
    bottomBarLayout->setContentsMargins(10,0,10,0);
    creditsLabel = new QLabel("By: Gemini | Theme: PyDracula | Powered by Qt");
    creditsLabel->setAlignment(Qt::AlignVCenter);
    bottomBarLayout->addWidget(creditsLabel);
    contentAreaLayout->addWidget(contentBottom);

    QFrame* topMenu = new QFrame();
    topMenu->setObjectName("topMenu");
    leftMenuLayout->addWidget(topMenu, 0, Qt::AlignTop);
    leftMenuLayout->addStretch();

    bottomMenu = new QFrame();
    bottomMenu->setObjectName("bottomMenu");
    QVBoxLayout* bottomMenuLayout = new QVBoxLayout(bottomMenu);
    bottomMenuLayout->setContentsMargins(0,0,0,0);
    bottomMenuLayout->setSpacing(0);
    settingsButton = new QPushButton("Settings");
    settingsButton->setObjectName("settingsButton");
    settingsButton->setMinimumHeight(60);
    bottomMenuLayout->addWidget(settingsButton);
    leftMenuLayout->addWidget(bottomMenu, 0, Qt::AlignBottom);

    QHBoxLayout *topButtonsLayout = new QHBoxLayout();
    setFixedRegionButton = new QPushButton("Set Fixed Region");
    translateFixedRegionButton = new QPushButton("Translate Fixed Region");
    singleTranslateButton = new QPushButton("Select & Translate");
    topButtonsLayout->addWidget(setFixedRegionButton);
    topButtonsLayout->addWidget(translateFixedRegionButton);
    topButtonsLayout->addWidget(singleTranslateButton);
    page1Layout->addLayout(topButtonsLayout);

    resultText = new QTextEdit();
    resultText->setReadOnly(true);
    resultText->setStyleSheet("font-size: 16pt;");
    page1Layout->addWidget(resultText);

    statusLabel = new QLabel("Ready");
    page1Layout->addWidget(statusLabel, 0, Qt::AlignRight);
}

MainWindow::~MainWindow() {}

void MainWindow::closeEvent(QCloseEvent *event) { saveSettings(); QMainWindow::closeEvent(event); }

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && contentTopBg->underMouse()) {
        isMovingWindow = true;
        oldMousePos = event->globalPosition().toPoint();
    }
}

void MainWindow::mouseMoveEvent(QMouseEvent *event) {
    if (isMovingWindow) {
        const QPoint delta = event->globalPosition().toPoint() - oldMousePos;
        move(x() + delta.x(), y() + delta.y());
        oldMousePos = event->globalPosition().toPoint();
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event) { isMovingWindow = false; }

void MainWindow::loadSettings()
{
    QSettings settings("ScreenTranslate", "ScreenTranslate");
    fixedRegion = settings.value("fixedRegion").toRect();
    if (fixedRegion.isValid()) {
        translateFixedRegionButton->setEnabled(true);
        statusLabel->setText(QString("Fixed region loaded: (%1, %2, %3x%4)").arg(fixedRegion.x()).arg(fixedRegion.y()).arg(fixedRegion.width()).arg(fixedRegion.height()));
    } else {
        translateFixedRegionButton->setEnabled(false);
    }

    // Settings are loaded in SettingsDialog, we just need to apply them
    applySettings();
    applyTheme();
}

void MainWindow::saveSettings()
{
    QSettings settings("ScreenTranslate", "ScreenTranslate");
    settings.setValue("fixedRegion", fixedRegion);
}

void MainWindow::applySettings()
{
    // Apply settings from the dialog to the rest of the application
    apiHandler->setProxy(settingsDialog->isProxyEnabled(), settingsDialog->proxyHost(), settingsDialog->proxyPort());
    applyTheme();
    // You can add other settings to be applied here, for example:
    // apiHandler->setApiKey(settingsDialog->apiKey());
    // apiHandler->setModel(settingsDialog->modelName());
}

void MainWindow::applyTheme()
{
    QString themeName = settingsDialog->themeName().toLower();
    QString qssPath;
    if (themeName == "light") {
        qssPath = ":/themes/py_dracula_light.qss";
    } else {
        qssPath = ":/themes/py_dracula_dark.qss";
    }

    QFile f(qssPath);
    if (!f.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream ts(&f);
    QString base = ts.readAll();

    QFile patchFile(":/themes/app_patch.qss");
    if (!patchFile.open(QIODevice::ReadOnly | QIODevice::Text)) return;
    QTextStream patchTs(&patchFile);
    QString patch = patchTs.readAll();

    // Qt6：把实例安全地转成 QApplication 再设样式
    if (auto *app = qobject_cast<QApplication*>(QCoreApplication::instance())) {
        app->setStyleSheet(base + "\n" + patch);
    }
}

void MainWindow::onSettingsClicked()
{
    if (settingsDialog->exec() == QDialog::Accepted) {
        applySettings();
    }
}

QMessageBox* MainWindow::createStyledMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text)
{
    auto msgBox = new QMessageBox(this);
    msgBox->setIcon(icon);
    msgBox->setWindowTitle(title);
    msgBox->setText(text);
    msgBox->setStyleSheet(qApp->styleSheet());
    return msgBox;
}

void MainWindow::startSnippet(CaptureMode mode) {
    if (settingsDialog->apiKey().isEmpty()) {
        createStyledMessageBox(QMessageBox::Warning, "API Key Missing", "Please set your API Key in Settings.")->exec();
        return;
    }
    currentCaptureMode = mode;
    windowBeingMoved = this->geometry(); // Save window geometry
    this->hide();
    snippetWidget = new ScreenSnippet();
    connect(snippetWidget, &ScreenSnippet::snippetTaken, this, &MainWindow::onSnippetTaken);
    snippetWidget->show();
}

void MainWindow::onSnippetTaken(QPixmap snippet, const QRect& region) {
    this->setGeometry(windowBeingMoved); // Restore window geometry
    this->show();
    if (currentCaptureMode == CaptureMode::Setting) {
        fixedRegion = region;
        translateFixedRegionButton->setEnabled(true);
        statusLabel->setText(QString("Fixed region set to: (%1, %2, %3x%4)").arg(region.x()).arg(region.y()).arg(region.width()).arg(region.height()));
        saveSettings();
    } else if (currentCaptureMode == CaptureMode::Single) {
        doTranslate(snippet);
    }
    if(snippetWidget) {
        snippetWidget->deleteLater();
        snippetWidget = nullptr;
    }
}

// ... (rest of the file is the same) ...

void MainWindow::onSetFixedRegionClicked() { startSnippet(CaptureMode::Setting); }
void MainWindow::onSingleTranslateClicked() { startSnippet(CaptureMode::Single); }

void MainWindow::onTranslateFixedRegionClicked() {
    if (!fixedRegion.isValid()) {
        createStyledMessageBox(QMessageBox::Warning, "No Region Set", "Please set a fixed region first.")->exec();
        return;
    }
    QPixmap snippet = QGuiApplication::primaryScreen()->grabWindow(0, fixedRegion.x(), fixedRegion.y(), fixedRegion.width(), fixedRegion.height());
    doTranslate(snippet);
}

void MainWindow::doTranslate(QPixmap snippet) {
    if (settingsDialog->apiKey().isEmpty()) {
        createStyledMessageBox(QMessageBox::Warning, "API Key Missing", "Please set your API Key in Settings.")->exec();
        return;
    }
    statusLabel->setText("Translating...");
    QList<QJsonObject> historyToSend;
    if (settingsDialog->isHistoryEnabled()) {
        historyToSend = conversationHistory;
    }
    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    snippet.save(&buffer, "PNG");
    QString base64Image = byteArray.toBase64();
    QJsonObject userTextPart, userImagePart, inlineData;
    userTextPart["text"] = settingsDialog->prompt();
    inlineData["mime_type"] = "image/png";
    inlineData["data"] = base64Image;
    QJsonArray currentUserParts; currentUserParts.append(userTextPart); currentUserParts.append(userImagePart);
    lastRequest_userPart = QJsonObject();
    lastRequest_userPart["role"] = "user";
    lastRequest_userPart["parts"] = currentUserParts;
    apiHandler->translateImage(snippet, settingsDialog->apiKey(), settingsDialog->prompt(), settingsDialog->modelName(), historyToSend);
}

void MainWindow::updateHistory(const QString& resultText) {
    if (!settingsDialog->isHistoryEnabled()) {
        conversationHistory.clear();
        return;
    }

    // Only add the text part of the prompt to the history, not the image.
    QJsonObject userContentForHistory;
    userContentForHistory["role"] = "user";
    QJsonObject userTextPart;
    userTextPart["text"] = settingsDialog->prompt();
    QJsonArray userParts;
    userParts.append(userTextPart);
    userContentForHistory["parts"] = userParts;
    conversationHistory.append(userContentForHistory);

    QJsonObject modelTextPart; modelTextPart["text"] = resultText;
    QJsonArray modelParts; modelParts.append(modelTextPart);
    QJsonObject modelContent; modelContent["role"] = "model"; modelContent["parts"] = modelParts;
    conversationHistory.append(modelContent);

    int maxItems = settingsDialog->historyLength() * 2;
    while (conversationHistory.size() > maxItems) {
        conversationHistory.removeFirst();
    }
}

void MainWindow::onTranslationReady(const QString& translatedText) {
    resultText->setText(translatedText);
    statusLabel->setText("Translation successful.");
    updateHistory(translatedText);
}

void MainWindow::onTranslationError(const QString& errorString) {
    resultText->setText("Error: " + errorString);
    statusLabel->setText("An error occurred.");
    this->show();
    if(snippetWidget) {
        snippetWidget->deleteLater();
        snippetWidget = nullptr;
    }
}

void MainWindow::handleApiTestRequest() {
    QString apiKey = settingsDialog->apiKey();
    if (apiKey.isEmpty()) {
        createStyledMessageBox(QMessageBox::Warning, "API Key Missing", "Please enter your API Key in Settings before testing.")->exec();
        return;
    }
    statusLabel->setText("Testing API connection...");
    apiHandler->setProxy(settingsDialog->isProxyEnabled(), settingsDialog->proxyHost(), settingsDialog->proxyPort());
    apiHandler->testApiConnection(apiKey);
}

void MainWindow::handleApiTestSuccess(const QString& message) {
    statusLabel->setText("API test successful.");
    createStyledMessageBox(QMessageBox::Information, "API Test Success", "Successfully connected to the Gemini API.\n" + message)->exec();
}

void MainWindow::handleApiTestFailed(const QString& error) {
    statusLabel->setText("API test failed.");
    createStyledMessageBox(QMessageBox::Warning, "API Test Failed", "Could not connect to the Gemini API.\n\nDetails: " + error)->exec();
}