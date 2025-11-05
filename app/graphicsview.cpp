// SPDX-FileCopyrightText: 2025 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#include "graphicsview.h"

#include "graphicsscene.h"
#include "settings.h"

#include <QDebug>
#include <QMouseEvent>
#include <QScrollBar>
#include <QMimeData>
#include <QImageReader>
#include <QStyleOptionGraphicsItem>

GraphicsView::GraphicsView(QWidget *parent)
    : QGraphicsView (parent)
{
    setDragMode(QGraphicsView::ScrollHandDrag);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setStyleSheet("background-color: rgba(0, 0, 0, 220);"
                  "border-radius: 3px;");
    setAcceptDrops(false);
    setCheckerboardEnabled(false);

    connect(horizontalScrollBar(), &QScrollBar::valueChanged, this, &GraphicsView::viewportRectChanged);
    connect(verticalScrollBar(), &QScrollBar::valueChanged, this, &GraphicsView::viewportRectChanged);
}

void GraphicsView::showFileFromPath(const QString &filePath)
{
    emit navigatorViewRequired(false, transform());

    if (filePath.endsWith(".svg")) {
        showSvg(filePath);
    } else {
        QImageReader imageReader(filePath);
        imageReader.setAutoTransform(true);
        imageReader.setDecideFormatFromContent(true);
        imageReader.setAllocationLimit(0);

        // Since if the image format / plugin does not support this feature, imageFormat() will returns an invalid format.
        // So we cannot use imageFormat() and check if it returns QImage::Format_Invalid to detect if we support the file.
        // QImage::Format imageFormat = imageReader.imageFormat();
        if (imageReader.format().isEmpty()) {
            showText(tr("File is not a valid image"));
        } else if (imageReader.supportsAnimation() && imageReader.imageCount() > 1) {
            showAnimated(filePath);
        } else if (!imageReader.canRead()) {
            showText(tr("Image data is invalid or currently unsupported"));
        } else {
            QImage && img = imageReader.read();
            if (img.isNull()) {
                showText(tr("Image data is invalid or currently unsupported"));
            } else {
                if (img.format() == QImage::Format_CMYK8888) {
                    img.convertToColorSpace(QColorSpace::SRgb);
                }
                img.setDevicePixelRatio(devicePixelRatioF());
                showImage(img);
            }
        }
    }
}

void GraphicsView::showImage(const QPixmap &pixmap)
{
    resetTransform();
    scene()->showImage(pixmap);
    displayScene();
}

void GraphicsView::showImage(const QImage &image)
{
    resetTransform();
    scene()->showImage(QPixmap::fromImage(image));
    displayScene();
}

void GraphicsView::showText(const QString &text)
{
    resetTransform();
    scene()->showText(text);
    displayScene();
}

void GraphicsView::showSvg(const QString &filepath)
{
    resetTransform();
    scene()->showSvg(filepath);
    displayScene();
}

void GraphicsView::showAnimated(const QString &filepath)
{
    resetTransform();
    scene()->showAnimated(filepath);
    displayScene();
}

GraphicsScene *GraphicsView::scene() const
{
    return qobject_cast<GraphicsScene*>(QGraphicsView::scene());
}

void GraphicsView::setScene(GraphicsScene *scene)
{
    return QGraphicsView::setScene(scene);
}

qreal GraphicsView::scaleFactor() const
{
    return QStyleOptionGraphicsItem::levelOfDetailFromTransform(transform());
}

void GraphicsView::resetTransform()
{
    if (!shouldAvoidTransform()) {
        QGraphicsView::resetTransform();
    }
}

void GraphicsView::zoomView(qreal scaleFactor)
{
    m_enableFitInView = false;
    m_longImageMode = false;
    scale(scaleFactor, scaleFactor);
    applyTransformationModeByScaleFactor();
    emit navigatorViewRequired(!isThingSmallerThanWindowWith(transform()), transform());
}

// This is always according to user's view.
// the direction of the rotation will NOT be clockwise because the y-axis points downwards.
void GraphicsView::rotateView(bool clockwise)
{
    resetScale();

    QTransform tf(0,                  clockwise ? 1 : -1, 0,
                  clockwise ? -1 : 1, 0,                  0,
                  0,                  0,                  1);
    tf = transform() * tf;
    setTransform(tf);
    
    // Apply transformation mode but don't emit navigator signal here
    // Let displayScene() handle the navigator visibility correctly
    applyTransformationModeByScaleFactor();
}

void GraphicsView::flipView(bool horizontal)
{
    QTransform tf(horizontal ? -1 : 1, 0,                   0,
                  0,                   horizontal ? 1 : -1, 0,
                  0,                   0,                   1);
    tf = transform() * tf;
    setTransform(tf);

    // Ensure the navigation view is also flipped.
    emit navigatorViewRequired(!isThingSmallerThanWindowWith(transform()), transform());
}

void GraphicsView::resetScale()
{
    setTransform(resetScale(transform()));
    applyTransformationModeByScaleFactor();
    emit navigatorViewRequired(!isThingSmallerThanWindowWith(transform()), transform());
}

void GraphicsView::fitInView(const QRectF &rect, Qt::AspectRatioMode aspectRadioMode)
{
    QGraphicsView::fitInView(rect, aspectRadioMode);
    applyTransformationModeByScaleFactor();
}

void GraphicsView::fitByOrientation(Qt::Orientation ori, bool scaleDownOnly)
{
    resetScale();

    QRectF viewRect = this->viewport()->rect();
    QRectF imageRect = transform().mapRect(sceneRect());
    QSize viewSize = viewRect.size().toSize();

    qreal ratio;

    if (ori == Qt::Horizontal) {
        // Horizontal fit means fit by width
        if (scaleDownOnly && imageRect.width() <= viewSize.width()) {
            // Image width already fits, no scaling needed
            ratio = 1;
        } else {
            ratio = viewRect.width() / imageRect.width();
        }
    } else {
        // Vertical fit means fit by height
        if (scaleDownOnly && imageRect.height() <= viewSize.height()) {
            // Image height already fits, no scaling needed
            ratio = 1;
        } else {
            ratio = viewRect.height() / imageRect.height();
        }
    }

    if (ratio != 1) {
        scale(ratio, ratio);
    }

    // Position the image correctly based on orientation with rotation consideration
    QRectF originalScene = sceneRect();
    QTransform currentTransform = transform();

    if (ori == Qt::Horizontal) {
        // For horizontal fit (fit by width), position at top (for tall images)
        // Find the scene point that corresponds to the top-center of the transformed image
        QPointF sceneTopCenter;

        if (qFuzzyIsNull(currentTransform.m12()) && qFuzzyIsNull(currentTransform.m21())) {
            // 0° or 180° rotation
            if (currentTransform.m11() > 0 && currentTransform.m22() > 0) {
                // 0° rotation: use original top-center
                sceneTopCenter = QPointF(originalScene.center().x(), originalScene.top());
            } else {
                // 180° rotation: the visual "top" is now at the scene bottom
                sceneTopCenter = QPointF(originalScene.center().x(), originalScene.bottom());
            }
        } else {
            // 90/270 degree rotation: the "top" in view corresponds to left/right in scene
            if (currentTransform.m12() > 0) {
                // 90 degree: top in view = left in scene
                sceneTopCenter = QPointF(originalScene.left(), originalScene.center().y());
            } else {
                // 270 degree: top in view = right in scene
                sceneTopCenter = QPointF(originalScene.right(), originalScene.center().y());
            }
        }
        centerOn(sceneTopCenter);
    } else {
        // For vertical fit (fit by height), position at left (for wide images)
        // Find the scene point that corresponds to the left-center of the transformed image
        QPointF sceneLeftCenter;

        if (qFuzzyIsNull(currentTransform.m12()) && qFuzzyIsNull(currentTransform.m21())) {
            // 0° or 180° rotation
            if (currentTransform.m11() > 0 && currentTransform.m22() > 0) {
                // 0° rotation: use original left-center
                sceneLeftCenter = QPointF(originalScene.left(), originalScene.center().y());
            } else {
                // 180° rotation: the visual "left" is now at the scene right
                sceneLeftCenter = QPointF(originalScene.right(), originalScene.center().y());
            }
        } else {
            // 90/270 degree rotation: the "left" in view corresponds to top/bottom in scene
            if (currentTransform.m21() > 0) {
                // 90 degree: left in view = top in scene
                sceneLeftCenter = QPointF(originalScene.center().x(), originalScene.top());
            } else {
                // 270 degree: left in view = bottom in scene
                sceneLeftCenter = QPointF(originalScene.center().x(), originalScene.bottom());
            }
        }
        centerOn(sceneLeftCenter);
    }

    m_enableFitInView = false;

    applyTransformationModeByScaleFactor();
    emit navigatorViewRequired(!isThingSmallerThanWindowWith(transform()), transform());
}

bool GraphicsView::isLongImage() const
{
    // Get the transformed image size (considering rotation and other transforms)
    QRectF transformedRect = transform().mapRect(sceneRect());
    QSizeF imageSize = transformedRect.size();
    
    if (imageSize.isEmpty()) return false;
    
    qreal aspectRatio = imageSize.width() / imageSize.height();
    
    // Check if aspect ratio exceeds 5:2 (wide) or 2:5 (tall)
    return aspectRatio > 2.5 || aspectRatio < 0.4;
}

void GraphicsView::fitLongImage()
{
    QRectF transformedRect = transform().mapRect(sceneRect());

    if (transformedRect.width() < transformedRect.height()) {
        fitByOrientation(Qt::Horizontal, true);
    } else {
        fitByOrientation(Qt::Vertical, true);
    }
}

void GraphicsView::displayScene()
{
    if (shouldAvoidTransform()) {
        emit navigatorViewRequired(!isThingSmallerThanWindowWith(transform()), transform());
        return;
    }

    // Check if should apply long image mode
    if (Settings::instance()->autoLongImageMode() && isLongImage()) {
        m_longImageMode = true;
        m_firstUserMediaLoaded = true;
        if (isSceneBiggerThanView()) fitLongImage();
        return;
    }

    if (isSceneBiggerThanView()) {
        // Do fit-in-view
        fitInView(sceneRect(), Qt::KeepAspectRatio);
        // After fitInView, the image should fit the window, so hide navigator
        emit navigatorViewRequired(false, transform());
    } else {
        // Image is already smaller than window, no navigator needed
        emit navigatorViewRequired(false, transform());
    }

    m_longImageMode = false;
    m_enableFitInView = true;
    m_firstUserMediaLoaded = true;
}

bool GraphicsView::isSceneBiggerThanView() const
{
    if (!isThingSmallerThanWindowWith(transform())) {
        return true;
    } else {
        return false;
    }
}

// Automately do fit in view when viewport(window) smaller than image original size.
void GraphicsView::setEnableAutoFitInView(bool enable)
{
    m_enableFitInView = enable;
}

void GraphicsView::setLongImageMode(bool enable)
{
    m_longImageMode = enable;
}

bool GraphicsView::avoidResetTransform() const
{
    return m_avoidResetTransform;
}

void GraphicsView::setAvoidResetTransform(bool avoidReset)
{
    m_avoidResetTransform = avoidReset;
}

inline double zeroOrOne(double number)
{
    return qFuzzyIsNull(number) ? 0 : (number > 0 ? 1 : -1);
}

// Note: this only works if we only have 90 degree based rotation
//       and no shear/translate.
QTransform GraphicsView::resetScale(const QTransform & orig)
{
    return QTransform(zeroOrOne(orig.m11()), zeroOrOne(orig.m12()),
                      zeroOrOne(orig.m21()), zeroOrOne(orig.m22()),
                      orig.dx(),             orig.dy());
}

void GraphicsView::toggleCheckerboard(bool invertCheckerboardColor)
{
    setCheckerboardEnabled(!m_checkerboardEnabled, invertCheckerboardColor);
}

void GraphicsView::mousePressEvent(QMouseEvent *event)
{
    if (shouldIgnoreMousePressMoveEvent(event)) {
        event->ignore();
        // blumia: return here, or the QMouseEvent event transparency won't
        //         work if we set a QGraphicsView::ScrollHandDrag drag mode.
        return;
    }

    return QGraphicsView::mousePressEvent(event);
}

void GraphicsView::mouseMoveEvent(QMouseEvent *event)
{
    if (shouldIgnoreMousePressMoveEvent(event)) {
        event->ignore();
    }

    return QGraphicsView::mouseMoveEvent(event);
}

void GraphicsView::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::ForwardButton || event->button() == Qt::BackButton) {
        event->ignore();
    } else {
        QGraphicsItem *item = itemAt(event->pos());
        if (!item) {
            event->ignore();
        }
    }

    return QGraphicsView::mouseReleaseEvent(event);
}

void GraphicsView::wheelEvent(QWheelEvent *event)
{
    event->ignore();
    // blumia: no need for calling parent method.
}

void GraphicsView::resizeEvent(QResizeEvent *event)
{
    if (m_longImageMode) {
        // In long image mode, reapply long image logic on resize
        // We directly apply the long image mode logic without rechecking
        // if we should enter long image mode, as the mode is already active
        fitLongImage();
    } else if (m_enableFitInView) {
        bool originalSizeSmallerThanWindow = isThingSmallerThanWindowWith(resetScale(transform()));
        if (originalSizeSmallerThanWindow && scaleFactor() >= 1) {
            // no longer need to do fitInView()
            // but we leave the m_enableFitInView value unchanged in case
            // user resize down the window again.
        } else if (originalSizeSmallerThanWindow && scaleFactor() < 1) {
            resetScale();
        } else {
            fitInView(sceneRect(), Qt::KeepAspectRatio);
        }
    } else {
        emit navigatorViewRequired(!isThingSmallerThanWindowWith(transform()), transform());
    }
    return QGraphicsView::resizeEvent(event);
}

bool GraphicsView::isThingSmallerThanWindowWith(const QTransform &transform) const
{
    return rect().size().expandedTo(transform.mapRect(sceneRect()).size().toSize())
            == rect().size();
}

bool GraphicsView::shouldIgnoreMousePressMoveEvent(const QMouseEvent *event) const
{
    if (event->buttons() == Qt::NoButton) {
        return true;
    }

    QGraphicsItem *item = itemAt(event->pos());
    if (!item) {
        return true;
    }

    if (isThingSmallerThanWindowWith(transform())) {
        return true;
    }

    return false;
}

void GraphicsView::setCheckerboardEnabled(bool enabled, bool invertColor)
{
    m_checkerboardEnabled = enabled;
    bool isLightCheckerboard = Settings::instance()->useLightCheckerboard() ^ invertColor;
    if (m_checkerboardEnabled) {
        // Prepare background check-board pattern
        QPixmap tilePixmap(0x20, 0x20);
        tilePixmap.fill(isLightCheckerboard ? QColor(220, 220, 220, 170) : QColor(35, 35, 35, 170));
        QPainter tilePainter(&tilePixmap);
        constexpr QColor color(45, 45, 45, 170);
        constexpr QColor invertedColor(210, 210, 210, 170);
        tilePainter.fillRect(0, 0, 0x10, 0x10, isLightCheckerboard ? invertedColor : color);
        tilePainter.fillRect(0x10, 0x10, 0x10, 0x10, isLightCheckerboard ? invertedColor : color);
        tilePainter.end();

        setBackgroundBrush(tilePixmap);
    } else {
        setBackgroundBrush(Qt::transparent);
    }
}

void GraphicsView::applyTransformationModeByScaleFactor()
{
    if (this->scaleFactor() < 1) {
        scene()->trySetTransformationMode(Qt::SmoothTransformation, this->scaleFactor());
    } else {
        scene()->trySetTransformationMode(Qt::FastTransformation, this->scaleFactor());
    }
}

bool GraphicsView::shouldAvoidTransform() const
{
    return m_firstUserMediaLoaded && m_avoidResetTransform;
}
