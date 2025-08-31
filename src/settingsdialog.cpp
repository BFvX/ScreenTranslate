#include "settingsdialog.h"
#include <QFile>
#include <QTextStream>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QDialogButtonBox>
#include <QSettings>
#include <QLabel>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>
#include <QTabWidget>
#include <QDialogButtonBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>

SettingsDialog::SettingsDialog(QWidget *parent)
    : FramelessDialog(parent)                 // 构造基类
{
    setWindowTitle("Settings");
    setModal(true);                           // 如需模式对话框

    // --- Widgets ---
    apiKeyEdit = new QLineEdit();
    apiKeyEdit->setEchoMode(QLineEdit::Password);

    modelComboBox = new QComboBox();
    modelComboBox->addItem("gemini-2.5-pro");
    modelComboBox->addItem("gemini-2.5-flash");
    modelComboBox->addItem("gemini-pro-vision");

    themeComboBox = new QComboBox();
    themeComboBox->addItem("Dark");
    themeComboBox->addItem("Light");

    promptEdit = new QLineEdit();

    proxyCheckBox = new QCheckBox("Use Proxy");
    proxyHostEdit = new QLineEdit();
    proxyPortEdit = new QLineEdit();

    testApiButton = new QPushButton("Test API Connection");

    historyCheckBox = new QCheckBox("Enable Conversation History");
    historySpinBox = new QSpinBox();
    historySpinBox->setRange(1, 20);
    historySpinBox->setSuffix(" items");

    // --- Layouts ---
    QTabWidget *tabWidget = new QTabWidget();

    // General
    auto *generalTab = new QWidget();
    auto *generalCard = new QFrame(generalTab);
    generalCard->setObjectName("Card");
    generalCard->setAttribute(Qt::WA_StyledBackground, true);

    auto *generalForm = new QFormLayout(generalCard);
    generalForm->addRow("API Key:", apiKeyEdit);
    generalForm->addRow("Model:", modelComboBox);
    generalForm->addRow("Theme:", themeComboBox);
    generalForm->addRow("Default Prompt:", promptEdit);

    auto *generalOuter = new QVBoxLayout(generalTab);
    generalOuter->addWidget(generalCard);
    generalOuter->addStretch();

    // Network
    auto *networkTab = new QWidget();
    auto *networkCard = new QFrame(networkTab);
    networkCard->setObjectName("Card");
    networkCard->setAttribute(Qt::WA_StyledBackground, true);

    auto *networkLayout = new QVBoxLayout(networkCard);
    networkLayout->addWidget(proxyCheckBox);
    auto *proxyForm = new QFormLayout();
    proxyForm->addRow("Proxy Host:", proxyHostEdit);
    proxyForm->addRow("Proxy Port:", proxyPortEdit);
    networkLayout->addLayout(proxyForm);
    networkLayout->addWidget(testApiButton);
    networkLayout->addStretch();

    auto *networkOuter = new QVBoxLayout(networkTab);
    networkOuter->addWidget(networkCard);
    networkOuter->addStretch();

    // History
    auto *historyTab = new QWidget();
    auto *historyCard = new QFrame(historyTab);
    historyCard->setObjectName("Card");
    historyCard->setAttribute(Qt::WA_StyledBackground, true);

    auto *historyLayout = new QVBoxLayout(historyCard);
    historyLayout->addWidget(historyCheckBox);
    auto *historyForm = new QFormLayout();
    historyForm->addRow("History Length (N):", historySpinBox);
    historyLayout->addLayout(historyForm);
    historyLayout->addStretch();

    auto *historyOuter = new QVBoxLayout(historyTab);
    historyOuter->addWidget(historyCard);
    historyOuter->addStretch();

    tabWidget->addTab(generalTab, "General");
    tabWidget->addTab(networkTab, "Network");
    tabWidget->addTab(historyTab, "History");

    auto *buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    // 把内容塞进 root 容器，再交给 FramelessDialog
    auto root = new QWidget;
    auto rootLayout = new QVBoxLayout(root);
    rootLayout->setContentsMargins(0,0,0,0);
    rootLayout->addWidget(tabWidget);
    rootLayout->addWidget(buttonBox);

    setCentralWidget(root);                   // 关键一步

    // 连接“测试 API”按钮到对外信号（见 .h 的 signals）
    connect(testApiButton, &QPushButton::clicked, this, &SettingsDialog::apiTestRequested);

    // 保存设置
    connect(this, &QDialog::accepted, this, &SettingsDialog::saveSettings);

    loadSettings();
}

void SettingsDialog::loadSettings()
{
    QSettings settings("ScreenTranslate", "ScreenTranslate"); // 建议使用公司和产品名

    setApiKey(settings.value("apiKey").toString());
    setModelName(settings.value("modelName", "gemini-1.5-pro").toString());
    setThemeName(settings.value("theme", "Dark").toString());
    setPrompt(settings.value("prompt").toString());
    setProxyEnabled(settings.value("proxy/enabled", false).toBool());
    setProxyHost(settings.value("proxy/host").toString());
    setProxyPort(settings.value("proxy/port").toInt());
    setHistoryEnabled(settings.value("history/enabled", true).toBool());
    setHistoryLength(settings.value("history/length", 10).toInt());
}

void SettingsDialog::saveSettings()
{
    QSettings settings("ScreenTranslate", "ScreenTranslate");

    settings.setValue("apiKey", apiKey());
    settings.setValue("modelName", modelName());
    settings.setValue("theme", themeName());
    settings.setValue("prompt", prompt());
    settings.setValue("proxy/enabled", isProxyEnabled());
    settings.setValue("proxy/host", proxyHost());
    settings.setValue("proxy/port", proxyPort());
    settings.setValue("history/enabled", isHistoryEnabled());
    settings.setValue("history/length", historyLength());
}

// --- Getters ---
QString SettingsDialog::apiKey() const { return apiKeyEdit->text().trimmed(); }
QString SettingsDialog::modelName() const { return modelComboBox->currentText(); }
QString SettingsDialog::prompt() const { return promptEdit->text().trimmed(); }
QString SettingsDialog::themeName() const { return themeComboBox->currentText(); }
bool SettingsDialog::isProxyEnabled() const { return proxyCheckBox->isChecked(); }
QString SettingsDialog::proxyHost() const { return proxyHostEdit->text().trimmed(); }
quint16 SettingsDialog::proxyPort() const { return proxyPortEdit->text().toUShort(); }
bool SettingsDialog::isHistoryEnabled() const { return historyCheckBox->isChecked(); }
int SettingsDialog::historyLength() const { return historySpinBox->value(); }

// --- Setters ---
void SettingsDialog::setApiKey(const QString &key) { apiKeyEdit->setText(key); }
void SettingsDialog::setModelName(const QString &model) { modelComboBox->setCurrentText(model); }
void SettingsDialog::setPrompt(const QString &prompt) { promptEdit->setText(prompt); }
void SettingsDialog::setThemeName(const QString &theme) { themeComboBox->setCurrentText(theme); }
void SettingsDialog::setProxyEnabled(bool enabled) { proxyCheckBox->setChecked(enabled); }
void SettingsDialog::setProxyHost(const QString &host) { proxyHostEdit->setText(host); }
void SettingsDialog::setProxyPort(quint16 port) { proxyPortEdit->setText(QString::number(port)); }
void SettingsDialog::setHistoryEnabled(bool enabled) { historyCheckBox->setChecked(enabled); }
void SettingsDialog::setHistoryLength(int length) { historySpinBox->setValue(length); }
