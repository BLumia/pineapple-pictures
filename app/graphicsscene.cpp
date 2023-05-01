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
#include <QMovie>
#include <QPainter>

class PGraphicsPixmapItem : public QGraphicsPixmapItem
{
public:
    PGraphicsPixmapItem(const QPixmap &pixmap, QGraphicsItem *parent = nullptr)
        : QGraphicsPixmapItem(pixmap, parent)
    {}

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
    QGraphicsSvgItem * svgItem = new QGraphicsSvgItem(filepath);
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
