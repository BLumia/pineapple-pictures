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
    QGraphicsPixmapItem * pixmapItem = this->addPixmap(pixmap);
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

bool GraphicsScene::trySetTransformationMode(Qt::TransformationMode mode)
{
    QGraphicsPixmapItem * pixmapItem = qgraphicsitem_cast<QGraphicsPixmapItem *>(m_theThing);
    if (pixmapItem) {
        pixmapItem->setTransformationMode(mode);
        return true;
    }

    return false;
}

QPixmap GraphicsScene::renderToPixmap()
{
    QPixmap pixmap(sceneRect().toRect().size());
    pixmap.fill(Qt::transparent);
    QPainter p(&pixmap);
    render(&p, sceneRect());

    return pixmap;
}
