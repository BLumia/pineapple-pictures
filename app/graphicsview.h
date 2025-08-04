// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

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

    void showFileFromPath(const QString &filePath);

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

    void displayScene();
    bool isSceneBiggerThanView() const;
    void setEnableAutoFitInView(bool enable = true);
    void setLongImageMode(bool enable = true);

    bool avoidResetTransform() const;
    void setAvoidResetTransform(bool avoidReset);

    static QTransform resetScale(const QTransform & orig);

    // Long image mode support
    bool isLongImage() const;
    void fitLongImage();

signals:
    void navigatorViewRequired(bool required, QTransform transform);
    void viewportRectChanged();

public slots:
    void toggleCheckerboard(bool invertCheckerboardColor = false);

private:
    void mousePressEvent(QMouseEvent * event) override;
    void mouseMoveEvent(QMouseEvent * event) override;
    void mouseReleaseEvent(QMouseEvent * event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    bool isThingSmallerThanWindowWith(const QTransform &transform) const;
    bool shouldIgnoreMousePressMoveEvent(const QMouseEvent *event) const;
    void setCheckerboardEnabled(bool enabled, bool invertColor = false);
    void applyTransformationModeByScaleFactor();

    inline bool shouldAvoidTransform() const;

    // Consider switch to 3 state for "no fit", "always fit" and "fit when view is smaller"?
    // ... or even more? e.g. "fit/snap width" things...
    // Currently it's "no fit" when it's false and "fit when view is smaller" when it's true.
    bool m_enableFitInView = false;
    bool m_longImageMode = false;
    bool m_avoidResetTransform = false;
    bool m_checkerboardEnabled = false;
    bool m_useLightCheckerboard = false;
    bool m_firstUserMediaLoaded = false;
};

#endif // GRAPHICSVIEW_H
