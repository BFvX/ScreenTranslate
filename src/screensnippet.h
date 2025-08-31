#ifndef SCREENSNIPPET_H
#define SCREENSNIPPET_H

#include <QWidget>
#include <QPixmap>
#include <QPoint>
#include <QPainter>
#include <QGuiApplication>
#include <QScreen>

class ScreenSnippet : public QWidget
{
    Q_OBJECT

public:
    explicit ScreenSnippet(QWidget *parent = nullptr);

signals:
    void snippetTaken(const QPixmap& snippet, const QRect& region);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;

private:
    QPixmap desktopPixmap;
    QPoint beginPoint;
    QPoint endPoint;
    bool isSelecting;
};

#endif // SCREENSNIPPET_H
