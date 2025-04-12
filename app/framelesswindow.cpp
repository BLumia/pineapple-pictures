// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
// SPDX-FileCopyrightText: 2023 Tad Young <yyc12321@outlook.com>
//
// SPDX-License-Identifier: MIT

#include "framelesswindow.h"

#include <QMouseEvent>
#include <QHoverEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QWindow>

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout(this))
    , m_oldCursorShape(Qt::ArrowCursor)
    , m_oldEdges()
{
    // this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
    this->setWindowFlags(Qt::ExpandedClientAreaHint | Qt::NoTitleBarBackgroundHint);
    this->setAttribute(Qt::WA_LayoutOnEntireRect);
    this->setMouseTracking(true);
    this->setAttribute(Qt::WA_Hover, true);
    this->installEventFilter(this);

    m_centralLayout->setContentsMargins(QMargins());
}

void FramelessWindow::setCentralWidget(QWidget *widget)
{
    if (m_centralWidget) {
        m_centralLayout->removeWidget(m_centralWidget);
        m_centralWidget->deleteLater();
    }

    m_centralLayout->addWidget(widget);
    m_centralWidget = widget;
}

void FramelessWindow::installResizeCapture(QObject* widget)
{
    widget->installEventFilter(this);
}

bool FramelessWindow::eventFilter(QObject* o, QEvent* e)
{
    switch (e->type()) {
    case QEvent::HoverMove:
    {
        QWidget* wg = qobject_cast<QWidget*>(o);
        if (wg != nullptr)
            return mouseHover(static_cast<QHoverEvent*>(e), wg);

        break;
    }
    case QEvent::MouseButtonPress:
        return mousePress(static_cast<QMouseEvent*>(e));
    }

    return QWidget::eventFilter(o, e);
}

bool FramelessWindow::mouseHover(QHoverEvent* event, QWidget* wg)
{
    if (!isMaximized() && !isFullScreen()) {
        QWindow* win = window()->windowHandle();
        Qt::Edges edges = this->getEdgesByPos(wg->mapToGlobal(event->oldPos()), win->frameGeometry());

        // backup & restore cursor shape
        if (edges && !m_oldEdges)
            // entering the edge. backup cursor shape
            m_oldCursorShape = win->cursor().shape();
        if (!edges && m_oldEdges)
            // leaving the edge. restore cursor shape
            win->setCursor(m_oldCursorShape);

        // save the latest edges status
        m_oldEdges = edges;

        // show resize cursor shape if cursor is within border
        if (edges) {
            win->setCursor(this->getCursorByEdge(edges, Qt::ArrowCursor));
            return true;
        }
    }

    return false;
}

bool FramelessWindow::mousePress(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton && !isMaximized() && !isFullScreen()) {
        QWindow* win = window()->windowHandle();
        Qt::Edges edges = this->getEdgesByPos(event->globalPosition().toPoint(), win->frameGeometry());
        if (edges) {
            win->startSystemResize(edges);
            return true;
        }
        else if (event->position().y() < contentsMargins().top()) {
            qDebug() << contentsMargins();
            win->startSystemMove();
            return true;
        }
    }

    return false;
}

Qt::CursorShape FramelessWindow::getCursorByEdge(const Qt::Edges& edges, Qt::CursorShape default_cursor)
{
    if ((edges == (Qt::TopEdge | Qt::LeftEdge)) || (edges == (Qt::RightEdge | Qt::BottomEdge)))
        return Qt::SizeFDiagCursor;
    else if ((edges == (Qt::TopEdge | Qt::RightEdge)) || (edges == (Qt::LeftEdge | Qt::BottomEdge)))
        return Qt::SizeBDiagCursor;
    else if (edges & (Qt::TopEdge | Qt::BottomEdge))
        return Qt::SizeVerCursor;
    else if (edges & (Qt::LeftEdge | Qt::RightEdge))
        return Qt::SizeHorCursor;
    else
        return default_cursor;
}

Qt::Edges FramelessWindow::getEdgesByPos(const QPoint gpos, const QRect& winrect)
{
    const int borderWidth = 8;
    Qt::Edges edges;

    int x = gpos.x() - winrect.x();
    int y = gpos.y() - winrect.y();

    if (x < borderWidth)
        edges |= Qt::LeftEdge;
    if (x > (winrect.width() - borderWidth))
        edges |= Qt::RightEdge;
    if (y < borderWidth)
        edges |= Qt::TopEdge;
    if (y > (winrect.height() - borderWidth))
        edges |= Qt::BottomEdge;

    return edges;
}

