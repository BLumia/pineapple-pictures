#ifndef GRAPHICSVIEW_H
#define GRAPHICSVIEW_H

#include <QGraphicsView>
#include <QUrl>

class GraphicsScene;
class GraphicsView : public QGraphicsView
{
    Q_OBJECT
public:
    GraphicsView(QWidget *parent = nullptr);

    void showFileFromPath(const QString &filePath, bool requestGallery = false);

    void showImage(const QPixmap &pixmap);
    void showImage(const QImage &image);
    void showText(const QString &text);
    void showSvg(const QString &filepath);
    void showGif(const QString &filepath);

    GraphicsScene * scene() const;
    void setScene(GraphicsScene *scene);

    qreal scaleFactor() const;

    void resetTransform();
    void zoomView(qreal scaleFactor);
    void resetScale();
    void rotateView(qreal rotateAngel);
    void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);

    void checkAndDoFitInView(bool markItOnAnyway = true);

signals:
    void navigatorViewRequired(bool required, qreal angle);
    void viewportRectChanged();
    void requestGallery(const QString &filePath);

public slots:
    void toggleCheckerboard(bool invertCheckerboardColor = false);

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
    void setCheckerboardEnabled(bool enabled, bool invertColor = false);
    void applyTransformationModeByScaleFactor();

    void resetWithScaleAndRotate(qreal scaleFactor, qreal rotateAngle);

    bool m_enableFitInView = false;
    bool m_checkerboardEnabled = false;
    bool m_isLastCheckerboardColorInverted = false;
    qreal m_rotateAngle = 0;
};

#endif // GRAPHICSVIEW_H
