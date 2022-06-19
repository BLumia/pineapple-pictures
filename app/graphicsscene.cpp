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
#include <QLabel>
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
        m_cachedPixmap = pixmap().scaled(
             resizedScale.toSize(),
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
    QLabel * label = new QLabel;
    QMovie * movie = new QMovie(filepath, QByteArray(), label);
    label->setStyleSheet("background-color:rgba(225,255,255,0);");
    label->setMovie(movie);
    this->addWidget(label);
    movie->start();
    m_theThing = this->addRect(QRect(QPoint(0, 0), label->sizeHint()),
                               QPen(Qt::transparent));
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
