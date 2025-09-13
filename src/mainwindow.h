#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QList>
#include <QJsonObject>
#include <QCloseEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QPixmap>
#include <QRect>
#include <QStringList>

// Forward declarations
class QPushButton;
class QTextEdit;
class QLabel;
class QFrame;
class QVBoxLayout;
class ScreenSnippet;
class GeminiApiHandler;
class SettingsDialog;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

enum class CaptureMode {
    Single,
    Setting
    };

protected:
    void closeEvent(QCloseEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private slots:
    void onSetFixedRegionClicked();
    void onTranslateFixedRegionClicked();
    void onSingleTranslateClicked();
    void onSettingsClicked();
    void onSnippetTaken(QPixmap snippet, const QRect& region);
    void onTranslationReady(const QString& translatedText);
    void onTranslationError(const QString& errorString);
    void handleApiTestRequest();
    void handleApiTestSuccess(const QString& message);
    void handleApiTestFailed(const QString& error);
    void handleAvailableModels(const QStringList &models);

private:
    void setupUI();
    void startSnippet(CaptureMode mode);
    void doTranslate(QPixmap snippet);
    void updateHistory(const QString& resultText);
    void loadSettings();
    void saveSettings();
    void applySettings();
    void applyTheme();
    QMessageBox* createStyledMessageBox(QMessageBox::Icon icon, const QString &title, const QString &text);

    // UI Elements
    QFrame *centralWidgetFrame;
    QFrame *contentBottom;
    QLabel *creditsLabel;
    QFrame *contentTopBg;
    QLabel *titleLabel;
    QFrame *rightButtons;
    QPushButton *minimizeButton;
    QPushButton *maximizeButton;
    QPushButton *closeButton;
    QFrame *leftMenuBg;
    QFrame *bottomMenu;
    QPushButton *settingsButton;
    QFrame *contentArea;
    QFrame *pagesContainer;
    QVBoxLayout* page1Layout;

    QPushButton *setFixedRegionButton;
    QPushButton *translateFixedRegionButton;
    QPushButton *singleTranslateButton;
    QTextEdit *resultText;
    QLabel *statusLabel;

    // Data & Logic
    ScreenSnippet *snippetWidget;
    GeminiApiHandler *apiHandler;
    SettingsDialog *settingsDialog;
    QRect fixedRegion;
    QRect windowBeingMoved;
    CaptureMode currentCaptureMode;
    QList<QJsonObject> conversationHistory;
    QJsonObject lastRequest_userPart;
    bool isMovingWindow;
    QPoint oldMousePos;
};

#endif // MAINWINDOW_H
