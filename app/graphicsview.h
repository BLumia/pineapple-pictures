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
    void showAnimated(const QString &filepath);

    GraphicsScene * scene() const;
    void setScene(GraphicsScene *scene);

    qreal scaleFactor() const;

    void resetTransform();
    void zoomView(qreal scaleFactor);
    void rotateView(bool clockwise = true);
    void flipView(bool horizontal = true);
    void resetScale();
    void fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode = Qt::IgnoreAspectRatio);
    void fitByOrientation(Qt::Orientation ori = Qt::Horizontal, bool scaleDownOnly = false);

    void checkAndDoFitInView(bool markItOnAnyway = true);

    static QTransform resetScale(const QTransform & orig);

signals:
    void navigatorViewRequired(bool required, QTransform transform);
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

    bool m_enableFitInView = false;
    bool m_checkerboardEnabled = false;
    bool m_isLastCheckerboardColorInverted = false;
};

#endif // GRAPHICSVIEW_H
