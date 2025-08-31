#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include "framelessdialog.h"

// Forward declarations
class QLineEdit;
class QComboBox;
class QCheckBox;
class QSpinBox;
class QPushButton;

class SettingsDialog : public FramelessDialog
{
    Q_OBJECT

public:
    explicit SettingsDialog(QWidget *parent = nullptr);

    // Getters
    QString apiKey() const;
    QString modelName() const;
    QString prompt() const;
    QString themeName() const;
    bool isProxyEnabled() const;
    QString proxyHost() const;
    quint16 proxyPort() const;
    bool isHistoryEnabled() const;
    int historyLength() const;

    // Setters
    void setApiKey(const QString &key);
    void setModelName(const QString &model);
    void setPrompt(const QString &prompt);
    void setThemeName(const QString &theme);
    void setProxyEnabled(bool enabled);
    void setProxyHost(const QString &host);
    void setProxyPort(quint16 port);
    void setHistoryEnabled(bool enabled);
    void setHistoryLength(int length);

signals:
    void apiTestRequested();

private:
    void loadSettings();
    void saveSettings();

private:
    QLineEdit *apiKeyEdit;
    QComboBox *modelComboBox;
    QLineEdit *promptEdit;
    QComboBox *themeComboBox;
    QCheckBox *proxyCheckBox;
    QLineEdit *proxyHostEdit;
    QLineEdit *proxyPortEdit;
    QPushButton *testApiButton;
    QCheckBox *historyCheckBox;
    QSpinBox *historySpinBox;
};

#endif // SETTINGSDIALOG_H