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
    void resizeEvent(QResizeEvent *event) override;

    void dragEnterEvent(QDragEnterEvent *event) override;
    void dragMoveEvent(QDragMoveEvent *event) override;
    void dropEvent(QDropEvent *event) override;

    bool isThingSmallerThanWindowWith(const QTransform &transform) const;
    bool shouldIgnoreMousePressMoveEvent(const QMouseEvent *event) const;

    bool m_enableFitInView = false;
};

#endif // GRAPHICSVIEW_H
