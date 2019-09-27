#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QWidget *parent = nullptr);

private:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void wheelEvent(QWheelEvent *event) override;
};

#endif // GRAPHICSVIEW_H
