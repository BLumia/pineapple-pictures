// SPDX-FileCopyrightText: 2025 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#include "titlebar.h"

#include "opacityhelper.h"

#include <QPainter>
#include <QStyle>
#include <QMouseEvent>
#include <QEvent>
#include <QEnterEvent>
#include <QWindow>
#include <QCursor>

TitleBar::TitleBar(QWidget *parent)
    : QWidget(parent)
    , m_opacityHelper(new OpacityHelper(this))
    , m_closeIcon(QStringLiteral(":/icons/window-close.svg"))
{
    setFixedHeight(32);
    setMouseTracking(true);
    setAttribute(Qt::WA_Hover, true);

    if (QWidget *win = window())
        win->installEventFilter(this);
}

void TitleBar::setOpacity(qreal opacity, bool animated)
{
    m_opacityHelper->setOpacity(opacity, animated);
}

void TitleBar::setCloseButtonVisible(bool visible)
{
    if (m_closeButtonVisible == visible)
        return;
    m_closeButtonVisible = visible;
    if (!visible) {
        m_closeHovered = false;
        m_closePressed = false;
    }
    update();
}

void TitleBar::setCloseButtonOnly(bool only)
{
    if (m_closeButtonOnly == only)
        return;
    m_closeButtonOnly = only;
    update();
}

QRect TitleBar::closeButtonRect() const
{
    const int btnWidth = closeButtonWidth();
    return QRect(width() - btnWidth, 0, btnWidth, height());
}

void TitleBar::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);

    // Subtle translucent backdrop so the title bar region is distinguishable
    // (similar to the bottom button group). Skipped in close-button-only mode.
    if (!m_closeButtonOnly) {
        painter.fillRect(rect(), QColor(0, 0, 0, 120));
    }

    const QRect closeRect = closeButtonRect();

    // Title text (leave room for the close button).
    QRect labelRect = rect().adjusted(8, 0, 0, 0);
    if (m_closeButtonVisible)
        labelRect.setRight(closeRect.left() - 2);

    const QString title = window() ? window()->windowTitle() : QString();
    if (!m_closeButtonOnly && !title.isEmpty()) {
        const QString elided = painter.fontMetrics().elidedText(title, Qt::ElideRight, labelRect.width());
        const int flags = Qt::AlignLeft | Qt::AlignVCenter | Qt::TextSingleLine;
        painter.setPen(Qt::black);
        painter.drawText(labelRect.adjusted(1, 1, 1, 1), flags, elided);
        painter.setPen(Qt::white);
        painter.drawText(labelRect, flags, elided);
    }

    if (m_closeButtonVisible) {
        if (m_closeHovered) {
            painter.fillRect(closeRect,
                             m_closePressed ? QColor(0xC5, 0x0F, 0x1F)
                                            : QColor(0xE8, 0x11, 0x23));
        }
        const int sz = height() / 3 * 2;
        const QRect iconRect = QStyle::alignedRect(layoutDirection(), Qt::AlignCenter,
                                                   QSize(sz, sz), closeRect);
        m_closeIcon.paint(&painter, iconRect);
    }
}

void TitleBar::mousePressEvent(QMouseEvent *event)
{
    if (event->button() != Qt::LeftButton) {
        QWidget::mousePressEvent(event);
        return;
    }

    if (m_closeButtonVisible && closeButtonRect().contains(event->pos())) {
        m_closePressed = true;
        m_dragPending = false;
        update();
        event->accept();
        return;
    }

    m_dragPending = true;
    m_moveStartPos = event->pos();
    event->accept();
}

void TitleBar::mouseMoveEvent(QMouseEvent *event)
{
    if (m_closeButtonVisible) {
        const bool hovered = closeButtonRect().contains(event->pos());
        if (hovered != m_closeHovered) {
            m_closeHovered = hovered;
            update();
        }
    }

    if (event->buttons() & Qt::LeftButton && m_dragPending
        && !window()->isMaximized() && !window()->isFullScreen()) {
        if (QWindow *wh = window()->windowHandle()) {
            if (!wh->startSystemMove())
                window()->move(event->globalPosition().toPoint() - m_moveStartPos);
        }
        event->accept();
    }

    QWidget::mouseMoveEvent(event);
}

void TitleBar::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        const bool wasClosePressed = m_closePressed;
        m_closePressed = false;
        m_dragPending = false;
        update();
        if (wasClosePressed && m_closeButtonVisible
            && closeButtonRect().contains(event->pos())) {
            emit closeRequested();
            event->accept();
            return;
        }
    }

    QWidget::mouseReleaseEvent(event);
}

void TitleBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton
        && !(m_closeButtonVisible && closeButtonRect().contains(event->pos()))) {
        emit maximizeToggleRequested();
        event->accept();
        return;
    }

    QWidget::mouseDoubleClickEvent(event);
}

void TitleBar::enterEvent(QEnterEvent *event)
{
    Q_UNUSED(event);
    if (m_closeButtonVisible) {
        const bool hovered = closeButtonRect().contains(mapFromGlobal(QCursor::pos()));
        if (hovered != m_closeHovered) {
            m_closeHovered = hovered;
            update();
        }
    }

    QWidget::enterEvent(event);
}

void TitleBar::leaveEvent(QEvent *event)
{
    if (m_closeHovered) {
        m_closeHovered = false;
        update();
    }

    QWidget::leaveEvent(event);
}

bool TitleBar::eventFilter(QObject *watched, QEvent *event)
{
    if (watched == window()) {
        switch (event->type()) {
        case QEvent::WindowTitleChange:
        case QEvent::WindowStateChange:
        case QEvent::ActivationChange:
            update();
            break;
        default:
            break;
        }
    }

    return QWidget::eventFilter(watched, event);
}

QSize TitleBar::sizeHint() const
{
    return QSize(0, 32);
}
