// SPDX-FileCopyrightText: 2025 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#ifndef TITLEBAR_H
#define TITLEBAR_H

#include <QWidget>
#include <QIcon>

QT_BEGIN_NAMESPACE
class QPaintEvent;
class QMouseEvent;
class QEvent;
class QEnterEvent;
QT_END_NAMESPACE

class OpacityHelper;

class TitleBar : public QWidget
{
    Q_OBJECT
public:
    explicit TitleBar(QWidget *parent = nullptr);

    void setOpacity(qreal opacity, bool animated = true);
    void setCloseButtonVisible(bool visible);
    bool closeButtonOnly() const { return m_closeButtonOnly; }
    void setCloseButtonOnly(bool only);
    int closeButtonWidth() const { return qMax(height(), 46); }

signals:
    void closeRequested();
    void maximizeToggleRequested();

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void enterEvent(QEnterEvent *event) override;
    void leaveEvent(QEvent *event) override;
    bool eventFilter(QObject *watched, QEvent *event) override;
    QSize sizeHint() const override;

private:
    QRect closeButtonRect() const;

    OpacityHelper *m_opacityHelper;
    QIcon m_closeIcon;
    bool m_closeButtonVisible = true;
    bool m_closeButtonOnly = false;
    bool m_closeHovered = false;
    bool m_closePressed = false;
    bool m_dragPending = false;
    QPoint m_moveStartPos;
};

#endif // TITLEBAR_H
