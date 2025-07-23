// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef NAVIGATORVIEW_H
#define NAVIGATORVIEW_H

#include <QGraphicsView>

class OpacityHelper;
class GraphicsView;
class NavigatorView : public QGraphicsView
{
    Q_OBJECT
public:
    NavigatorView(QWidget *parent = nullptr);

    void setMainView(GraphicsView *mainView);
    void setOpacity(qreal opacity, bool animated = true);

public slots:
    void updateMainViewportRegion();

private:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;

    void wheelEvent(QWheelEvent *event) override;
    void paintEvent(QPaintEvent *event) override;

    bool m_mouseDown = false;
    QPolygon m_viewportRegion;
    QGraphicsView *m_mainView = nullptr;
    OpacityHelper *m_opacityHelper = nullptr;
};

#endif // NAVIGATORVIEW_H
