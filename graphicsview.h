#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QUrl>

class GraphicsScene;
class GraphicsView : public QGraphicsView
{
public:
    GraphicsView(QWidget *parent = nullptr);

    void showFromUrlList(const QList<QUrl> &urlList);

    void showImage(const QPixmap &pixmap);
    void showText(const QString &text);
    void showSvg(const QString &filepath);
    void showGif(const QString &filepath);

    GraphicsScene * scene() const;
    void setScene(GraphicsScene *scene);

    void checkAndDoFitInView();

public slots:
    void toggleCheckerboard();

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
    void setCheckerboardEnabled(bool enabled);

    bool m_enableFitInView = false;
    bool m_checkerboardEnabled = false;
};

#endif // GRAPHICSVIEW_H
