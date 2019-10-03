#include "navigatorview.h"

#include <QMouseEvent>

NavigatorView::NavigatorView(QWidget *parent)
    : QGraphicsView (parent)
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("background-color: rgba(0, 0, 0, 220);"
                  "border-radius: 3px;");
}

void NavigatorView::wheelEvent(QWheelEvent *event)
{
    event->ignore();
    return QGraphicsView::wheelEvent(event);
}
