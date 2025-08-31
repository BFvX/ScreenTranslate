#include "titlebar.h"
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QMouseEvent>
#include <QApplication>
#include <QWindow>

TitleBar::TitleBar(QWidget* parent) : QWidget(parent) {
    setObjectName("TitleBar");
    setFixedHeight(36);                    // 你可改高

    m_icon  = new QLabel;
    m_title = new QLabel("Settings");

    // 简单用文本按钮，便于 QSS 全控
    m_minBtn   = new QPushButton("—");   // U+2014
    m_maxBtn   = new QPushButton("□");
    m_closeBtn = new QPushButton("×");

    for (auto* b : {m_minBtn, m_maxBtn, m_closeBtn}) {
        b->setObjectName("TitleButton");
        b->setFixedSize(36, 28);
        b->setFlat(true);
        b->setFocusPolicy(Qt::NoFocus);
    }
    m_closeBtn->setProperty("type", "close");
    m_maxBtn->setProperty("type", "max");
    m_minBtn->setProperty("type", "min");

    auto lay = new QHBoxLayout(this);
    lay->setContentsMargins(10, 4, 6, 4);
    lay->setSpacing(4);
    lay->addWidget(m_icon);
    lay->addSpacing(6);
    lay->addWidget(m_title, 1);
    lay->addWidget(m_minBtn);
    lay->addWidget(m_maxBtn);
    lay->addWidget(m_closeBtn);

    connect(m_minBtn, &QPushButton::clicked, this, &TitleBar::minimizeRequested);
    connect(m_maxBtn, &QPushButton::clicked, this, &TitleBar::maximizeToggleRequested);
    connect(m_closeBtn,&QPushButton::clicked, this, &TitleBar::closeRequested);
}

void TitleBar::setWindowTitleText(const QString& t) { m_title->setText(t); }
void TitleBar::setWindowIconPixmap(const QPixmap& pm) {
    m_icon->setPixmap(pm.scaled(16,16, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

void TitleBar::mousePressEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) m_dragPos = e->globalPosition();
    QWidget::mousePressEvent(e);
}
void TitleBar::mouseMoveEvent(QMouseEvent* e) {
    if (e->buttons() & Qt::LeftButton) {
        const QPointF delta = e->globalPosition() - m_dragPos;
        m_dragPos = e->globalPosition();
        if (auto* w = window()) w->move(w->pos() + delta.toPoint());
    }
    QWidget::mouseMoveEvent(e);
}
void TitleBar::mouseDoubleClickEvent(QMouseEvent* e) {
    if (e->button() == Qt::LeftButton) emit maximizeToggleRequested();
    QWidget::mouseDoubleClickEvent(e);
}