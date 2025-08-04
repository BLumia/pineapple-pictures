// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "navigatorview.h"

#include "graphicsview.h"
#include "opacityhelper.h"

#include <QMouseEvent>
#include <QDebug>
#include <QTimer>

NavigatorView::NavigatorView(QWidget *parent)
    : QGraphicsView (parent)
    , m_viewportRegion(this->rect())
    , m_opacityHelper(new OpacityHelper(this))
{
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setStyleSheet("background-color: rgba(0, 0, 0, 120);"
                  "border-radius: 3px;");
}

// doesn't take or manage its ownership
void NavigatorView::setMainView(GraphicsView *mainView)
{
    m_mainView = mainView;
}

void NavigatorView::setOpacity(qreal opacity, bool animated)
{
    m_opacityHelper->setOpacity(opacity, animated);
}

void NavigatorView::updateMainViewportRegion()
{
    // Use QTimer::singleShot with lambda to delay the update
    // This ensures all geometry updates are complete before calculating viewport region
    QTimer::singleShot(0, [this]() {
        if (m_mainView != nullptr) {
            m_viewportRegion = mapFromScene(m_mainView->mapToScene(m_mainView->rect()));
            update();
        }
    });
}

void NavigatorView::mousePressEvent(QMouseEvent *event)
{
    m_mouseDown = true;

    if (m_mainView) {
        m_mainView->centerOn(mapToScene(event->pos()));
        update();
    }

    event->accept();
}

void NavigatorView::mouseMoveEvent(QMouseEvent *event)
{
    if (m_mouseDown && m_mainView) {
        m_mainView->centerOn(mapToScene(event->pos()));
        update();
        event->accept();
    } else {
        event->ignore();
    }
}

void NavigatorView::mouseReleaseEvent(QMouseEvent *event)
{
    m_mouseDown = false;

    event->accept();
}

void NavigatorView::wheelEvent(QWheelEvent *event)
{
    event->ignore();
    return QGraphicsView::wheelEvent(event);
}

void NavigatorView::paintEvent(QPaintEvent *event)
{
    QGraphicsView::paintEvent(event);

    QPainter painter(viewport());
    painter.setPen(QPen(Qt::gray, 2));
    painter.drawRect(m_viewportRegion.boundingRect());
}
