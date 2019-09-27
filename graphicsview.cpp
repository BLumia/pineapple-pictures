#include "graphicsview.h"

#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView (parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("background-color: rgba(0, 0, 0, 180);"
                  "border-radius: 3px;");
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (!item) {
        event->ignore();
        // blumia: return here, or the QMouseEvent event transparency won't
        //         work if we set a QGraphicsView::ScrollHandDrag drag mode.
        return;
    }

    qDebug() << item;

    return QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (!item) {
        event->ignore();
    }

    return QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    QGraphicsItem *item = itemAt(event->pos());
    if (!item) {
        event->ignore();
    }

    return QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    if(event->delta() > 0) {
        scale(1.25, 1.25);
    } else {
        scale(0.8, 0.8);
    }
}
