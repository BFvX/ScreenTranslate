#include "screensnippet.h"
#include <QMouseEvent>
#include <QScreen>
#include <QGuiApplication>

ScreenSnippet::ScreenSnippet(QWidget *parent) : QWidget(parent), isSelecting(false)
{
    setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    QScreen *screen = QGuiApplication::primaryScreen();
    desktopPixmap = screen->grabWindow(0);
    resize(screen->geometry().size());
}

void ScreenSnippet::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
    painter.drawPixmap(0, 0, desktopPixmap);

    if (isSelecting) {
        QRect selectionRect(beginPoint, endPoint);
        painter.fillRect(selectionRect, QColor(0, 0, 0, 100)); // Darken the selected area slightly
        painter.setPen(Qt::red);
        painter.drawRect(selectionRect);
    }
}

void ScreenSnippet::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        beginPoint = event->pos();
        endPoint = event->pos();
        isSelecting = true;
        update();
    }
}

void ScreenSnippet::mouseMoveEvent(QMouseEvent *event)
{
    if (isSelecting) {
        endPoint = event->pos();
        update();
    }
}

void ScreenSnippet::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && isSelecting) {
        isSelecting = false;
        QRect selectionRect = QRect(beginPoint, endPoint).normalized();
        QPixmap snippet = desktopPixmap.copy(selectionRect);
        emit snippetTaken(snippet, selectionRect);
        close();
    }
}
