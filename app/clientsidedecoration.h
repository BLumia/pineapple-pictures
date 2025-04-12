#pragma once

#include "opacityhelper.h"

#include <QWidget>

class ClientSideDecoration : public QWidget
{
    Q_OBJECT
public:
    explicit ClientSideDecoration(QWidget *parent = nullptr);
    ~ClientSideDecoration();

    void setOpacity(qreal opacity, bool animated = true);

signals:
    void windowCloseRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void leaveEvent(QEvent *event) override;

private:
    OpacityHelper * m_opacityHelper;
    bool m_closeBtnHovered;
};
