// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "framelesswindow.h"

#include <QMouseEvent>
#include <QHoverEvent>
#include <QApplication>
#include <QVBoxLayout>
#include <QWindow>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout(this))
    , m_oldCursorShape(Qt::ArrowCursor)
    , m_oldEdges()
{
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    // The Qt::WindowMinMaxButtonsHint or Qt::WindowMinimizeButtonHint here is to
    // provide the ability to use Winkey + Up/Down to toggle minimize/maximize.
    // But a bug introduced in Qt6 that this flag will break the WM_NCHITTEST event.
    // See: QTBUG-112356 and discussion in https://github.com/BLumia/pineapple-pictures/pull/81
    // Thanks @yyc12345 for finding out the source of the issue.
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinMaxButtonsHint);
#else
    // There is a bug in Qt 5 that will make pressing Meta+Up cause the app
    // fullscreen under Windows, see QTBUG-91226 to learn more.
    // The bug seems no longer exists in Qt 6 (I only tested it under Qt 6.3.0).
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);
#endif // QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
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

/*
 * The references of following functions
 * https://stackoverflow.com/questions/411823/how-do-i-implement-qhoverevent-in-qt
 * https://stackoverflow.com/questions/74155493/how-can-i-resize-frameless-window-in-qml
 * https://gist.github.com/Nico-Duduf/b8c799f1f2a694732abd1238843b1d70
 * https://stackoverflow.com/questions/2445997/qgraphicsview-and-eventfilter
*/

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

    return false;
}

bool FramelessWindow::mousePress(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton && !isMaximized()) {
        QWindow* win = window()->windowHandle();
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        Qt::Edges edges = this->getEdgesByPos(event->globalPosition().toPoint(), win->frameGeometry());
#else
        Qt::Edges edges = this->getEdgesByPos(event->globalPos(), win->frameGeometry());
#endif // QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
        if (edges) {
            win->startSystemResize(edges);
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

