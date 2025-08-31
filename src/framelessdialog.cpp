#include "framelessdialog.h"
#include "titlebar.h"
#include <QVBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QApplication>
#include <QScreen>
#include <QWindow>

FramelessDialog::FramelessDialog(QWidget* parent) : QDialog(parent) {
    setWindowFlags(Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground, true);      // 让圆角外透明
    setObjectName("FramelessDialog");

    // 外层透明，内层 m_frame 才有背景与圆角
    m_frame = new QWidget;
    m_frame->setObjectName("DialogFrame");

    auto shadow = new QGraphicsDropShadowEffect(m_frame);
    shadow->setBlurRadius(30);
    shadow->setOffset(0, 8);
    shadow->setColor(QColor(0,0,0,160));
    m_frame->setGraphicsEffect(shadow);

    m_titleBar = new TitleBar;
    m_central  = new QWidget;
    m_central->setObjectName("DialogCentral");

    auto frameLayout = new QVBoxLayout(m_frame);
    frameLayout->setContentsMargins(8,8,8,8);              // 让阴影能显示
    frameLayout->setSpacing(0);
    frameLayout->addWidget(m_titleBar);
    frameLayout->addWidget(m_central, 1);

    auto outer = new QVBoxLayout(this);
    outer->setContentsMargins(12,12,12,12);                // 透明边距
    outer->addWidget(m_frame);

    connect(m_titleBar, &TitleBar::minimizeRequested, this, &FramelessDialog::onMinimize);
    connect(m_titleBar, &TitleBar::maximizeToggleRequested, this, &FramelessDialog::onMaximizeToggle);
    connect(m_titleBar, &TitleBar::closeRequested, this, &FramelessDialog::onClose);

    // 同步标题/图标
    connect(this, &QDialog::windowTitleChanged,
            m_titleBar, &TitleBar::setWindowTitleText);
}

void FramelessDialog::setCentralWidget(QWidget* w) {
    if (!w) return;
    if (w->parent() != m_central) w->setParent(m_central);
    auto lay = m_central->layout();
    if (!lay) {
        lay = new QVBoxLayout(m_central);
        lay->setContentsMargins(12,12,12,12);
    }
    // 清空旧内容（如有）
    while (auto item = lay->takeAt(0)) {
        if (auto wid = item->widget()) wid->deleteLater();
        delete item;
    }
    lay->addWidget(w);
}
QWidget* FramelessDialog::centralWidget() const { return m_central; }

void FramelessDialog::showEvent(QShowEvent* e) {
    QDialog::showEvent(e);
    m_titleBar->setWindowTitleText(windowTitle());
    if (!windowIcon().isNull())
        m_titleBar->setWindowIconPixmap(windowIcon().pixmap(16,16));
}

void FramelessDialog::onMinimize()      { setWindowState(windowState() | Qt::WindowMinimized); }
void FramelessDialog::onMaximizeToggle(){
    if (windowState() & Qt::WindowMaximized)
        setWindowState(Qt::WindowNoState);
    else
        setWindowState(windowState() | Qt::WindowMaximized);
}
void FramelessDialog::onClose()         { close(); }