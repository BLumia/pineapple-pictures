#include "clientsidedecoration.h"

#include <QMouseEvent>
#include <QPainter>

ClientSideDecoration::ClientSideDecoration(QWidget *parent)
    : QWidget{parent}
    , m_opacityHelper(new OpacityHelper(this))
{
    setFixedHeight(40);
    setMouseTracking(true);
}

ClientSideDecoration::~ClientSideDecoration()
{
}

void ClientSideDecoration::setOpacity(qreal opacity, bool animated)
{
    m_opacityHelper->setOpacity(opacity, animated);
}

void ClientSideDecoration::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(Qt::white);
    painter.drawText(rect().adjusted(15, 0, -50, 0), Qt::AlignVCenter, "卧槽");
    // painter.fillRect(rect(), QColor(0, 0, 0, 120));
    if (m_closeBtnHovered) painter.fillRect(rect().adjusted(width() - 50, 0, 0, 0), Qt::red);
    painter.drawText(rect().adjusted(width() - 50, 0, 0, 0), Qt::AlignCenter, "X");
}

void ClientSideDecoration::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (window()->isMaximized()) window()->showNormal();
    else window()->showMaximized();
}

void ClientSideDecoration::mouseMoveEvent(QMouseEvent *event)
{
    bool oldHoverState = m_closeBtnHovered;
    if (width() - event->position().x() <= 50) {
        m_closeBtnHovered = true;
    } else {
        m_closeBtnHovered = false;
    }
    if (oldHoverState != m_closeBtnHovered) update();
    return QWidget::mouseMoveEvent(event);
}

void ClientSideDecoration::mouseReleaseEvent(QMouseEvent *event)
{
    if (m_closeBtnHovered) emit windowCloseRequested();
    return QWidget::mouseReleaseEvent(event);
}

void ClientSideDecoration::leaveEvent(QEvent *event)
{
    m_closeBtnHovered = false;
    update();
    return QWidget::leaveEvent(event);
}
