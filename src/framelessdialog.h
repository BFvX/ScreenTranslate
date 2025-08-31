#pragma once
#include <QDialog>

class TitleBar;
class QVBoxLayout;

class FramelessDialog : public QDialog {
    Q_OBJECT
public:
    explicit FramelessDialog(QWidget* parent = nullptr);
    void   setCentralWidget(QWidget* w);
    QWidget* centralWidget() const;

protected:
    void showEvent(QShowEvent* e) override;

private slots:
    void onMinimize();
    void onMaximizeToggle();
    void onClose();

private:
    QWidget*   m_frame = nullptr;     // 圆角+背景+阴影的容器
    TitleBar*  m_titleBar = nullptr;
    QWidget*   m_central = nullptr;
};