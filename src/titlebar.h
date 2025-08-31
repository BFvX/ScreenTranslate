#pragma once
#include <QWidget>

class QLabel;
class QPushButton;

class TitleBar : public QWidget {
    Q_OBJECT
public:
    explicit TitleBar(QWidget* parent = nullptr);
    void setWindowTitleText(const QString& t);
    void setWindowIconPixmap(const QPixmap& pm);

signals:
    void minimizeRequested();
    void maximizeToggleRequested();
    void closeRequested();

protected:
    void mousePressEvent(QMouseEvent* e) override;
    void mouseMoveEvent(QMouseEvent* e) override;
    void mouseDoubleClickEvent(QMouseEvent* e) override;

private:
    QLabel*      m_icon = nullptr;
    QLabel*      m_title = nullptr;
    QPushButton* m_minBtn = nullptr;
    QPushButton* m_maxBtn = nullptr;
    QPushButton* m_closeBtn = nullptr;
    QPointF      m_dragPos;
};