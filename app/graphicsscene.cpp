// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "graphicsscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QDebug>
#include <QGraphicsItem>
#include <QUrl>
#include <QGraphicsSvgItem>
#include <QSvgRenderer>
#include <QMovie>
#include <QPainter>

class PGraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
    PGraphicsPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr)
        : QGraphicsPixmapItem(pixmap, parent)
    {}

    enum { Type = UserType + 1 };
    int type() const override { return Type; }

    void setScaleHint(float scaleHint) {
        m_scaleHint = scaleHint;
    }

    const QPixmap & scaledPixmap(float scaleHint) {
        if (qFuzzyCompare(scaleHint, m_cachedScaleHint)) return m_cachedPixmap;
        QSizeF resizedScale(boundingRect().size());
        resizedScale *= scaleHint;
        QPixmap && sourcePixmap = pixmap();
        m_cachedPixmap = sourcePixmap.scaled(
             resizedScale.toSize() * sourcePixmap.devicePixelRatioF(),
             Qt::KeepAspectRatio,
             Qt::SmoothTransformation);
        m_cachedScaleHint = scaleHint;
        return m_cachedPixmap;
    }

    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget) override
    {
        if (transformationMode() == Qt::FastTransformation) {
            return QGraphicsPixmapItem::paint(painter, option, widget);
        } else {
//            painter->setRenderHints(QPainter::Antialiasing);
            painter->drawPixmap(QRectF(offset(), boundingRect().size()).toRect(),
                                scaledPixmap(m_scaleHint));
        }
    }

private:
    float m_scaleHint = 1;
    float m_cachedScaleHint = -1;
    QPixmap m_cachedPixmap;
};

class PGraphicsMovieItem : public QGraphicsItem
{
public:
    PGraphicsMovieItem(QGraphicsItem *parent = nullptr) : QGraphicsItem(parent) {}

    enum { Type = UserType + 2 };
    int type() const override { return Type; }

    void setMovie(QMovie* movie) {
        if (m_movie) m_movie->disconnect();
        m_movie.reset(movie);
        m_movie->connect(m_movie.data(), &QMovie::updated, [this](){
            this->update();
        });
    }

    QRectF boundingRect() const override {
        if (m_movie) { return m_movie->frameRect(); }
        else { return QRectF(); }
    }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override {
        if (m_movie) {
            painter->drawPixmap(m_movie->frameRect(), m_movie->currentPixmap(), m_movie->frameRect());
        }
    }

    inline QMovie * movie() const {
        return m_movie.data();
    }

private:
    QScopedPointer<QMovie> m_movie;
};

GraphicsScene::GraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{
    showText(tr("Drag image here"));
}

GraphicsScene::~GraphicsScene()
{

}

void GraphicsScene::showImage(const QPixmap &pixmap)
{
    this->clear();
    PGraphicsPixmapItem * pixmapItem = new PGraphicsPixmapItem(pixmap);
    this->addItem(pixmapItem);
    pixmapItem->setShapeMode(QGraphicsPixmapItem::BoundingRectShape);
    m_theThing = pixmapItem;
    this->setSceneRect(m_theThing->boundingRect());
}

void GraphicsScene::showText(const QString &text)
{
    this->clear();
    QGraphicsTextItem * textItem = this->addText(text);
    textItem->setDefaultTextColor(QColor("White"));
    m_theThing = textItem;
    this->setSceneRect(m_theThing->boundingRect());
}

void GraphicsScene::showSvg(const QString &filepath)
{
    this->clear();
    QGraphicsSvgItem * svgItem = new QGraphicsSvgItem();
    QSvgRenderer * render = new QSvgRenderer(svgItem);
#if QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    // Qt 6.7.0's SVG support is terrible caused by huge memory usage, see QTBUG-124287
    // Qt 6.7.1's is somewhat better, memory issue seems fixed, but still laggy when zoom in,
    // see QTBUG-126771. Anyway let's disable it for now.
    render->setOptions(QtSvg::Tiny12FeaturesOnly);
#endif // QT_VERSION >= QT_VERSION_CHECK(6, 7, 0)
    render->load(filepath);
    svgItem->setSharedRenderer(render);
    this->addItem(svgItem);
    m_theThing = svgItem;
    this->setSceneRect(m_theThing->boundingRect());
}

void GraphicsScene::showAnimated(const QString &filepath)
{
    this->clear();

    PGraphicsMovieItem * animatedItem = new PGraphicsMovieItem();
    QMovie * movie = new QMovie(filepath);
    movie->start();
    animatedItem->setMovie(movie);
    this->addItem(animatedItem);
    m_theThing = animatedItem;

    this->setSceneRect(m_theThing->boundingRect());
}

bool GraphicsScene::trySetTransformationMode(Qt::TransformationMode mode, float scaleHint)
{
    PGraphicsPixmapItem * pixmapItem = qgraphicsitem_cast<PGraphicsPixmapItem *>(m_theThing);
    if (pixmapItem) {
        pixmapItem->setTransformationMode(mode);
        pixmapItem->setScaleHint(scaleHint);
        return true;
    }

    return false;
}

bool GraphicsScene::togglePauseAnimation()
{
    PGraphicsMovieItem * animatedItem = qgraphicsitem_cast<PGraphicsMovieItem *>(m_theThing);
    if (animatedItem) {
        animatedItem->movie()->setPaused(animatedItem->movie()->state() != QMovie::Paused);
        return true;
    }
    return false;
}

bool GraphicsScene::skipAnimationFrame(int delta)
{
    PGraphicsMovieItem * animatedItem = qgraphicsitem_cast<PGraphicsMovieItem *>(m_theThing);
    if (animatedItem) {
        const int frameCount = animatedItem->movie()->frameCount();
        const int currentFrame = animatedItem->movie()->currentFrameNumber();
        const int targetFrame = (currentFrame + delta) % frameCount;
        animatedItem->movie()->setPaused(true);
        return animatedItem->movie()->jumpToFrame(targetFrame);
    }
    return false;
}

QPixmap GraphicsScene::renderToPixmap()
{
    PGraphicsPixmapItem * pixmapItem = qgraphicsitem_cast<PGraphicsPixmapItem *>(m_theThing);
    if (pixmapItem) {
        return pixmapItem->pixmap();
    }

    QPixmap pixmap(sceneRect().toRect().size());
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    render(&p, sceneRect());

    return pixmap;
}
