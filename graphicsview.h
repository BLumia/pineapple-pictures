#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>

class GraphicsScene;
class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QWidget *parent = nullptr);

    void showImage(const QPixmap &pixmap);
    void showText(const QString &text);

    GraphicsScene * scene() const;
    void setScene(GraphicsScene *scene);

private:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void wheelEvent(QWheelEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;
};

#endif // GRAPHICSVIEW_H
