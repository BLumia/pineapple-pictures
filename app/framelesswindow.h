// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    typedef qintptr NATIVE_RESULT;
#else
    typedef long NATIVE_RESULT;
#endif // QT_VERSION_CHECK(6, 0, 0)

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class FramelessWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *parent = nullptr);

    void setCentralWidget(QWidget * widget);

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
protected:
    bool event(QEvent* e) override;

protected slots:
    void mousePressEvent(QMouseEvent *event) override;
#endif

private:
#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    Qt::Edges m_oldEdges;
    Qt::CursorShape m_oldCursorShape;

    void hoverMove(QHoverEvent* event);

    Qt::CursorShape getCursorByEdge(const Qt::Edges& edges, Qt::CursorShape default_cursor);
    Qt::Edges getEdgesByPos(const QPoint pos, const QRect& winrect);
#endif

    QVBoxLayout * m_centralLayout = nullptr;
    QWidget * m_centralWidget = nullptr; // just a pointer, doesn't take the ownership.
};

#endif // FRAMELESSWINDOW_H
