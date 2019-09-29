#include "graphicsscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QDebug>
#include <QGraphicsItem>
#include <QUrl>
#include <QGraphicsSvgItem>

GraphicsScene::GraphicsScene(QObject *parent)
    : QGraphicsScene(parent)
{
    showText("Drag image here");
}

GraphicsScene::~GraphicsScene()
{

}

void GraphicsScene::showImage(const QPixmap &pixmap)
{
    this->clear();
    m_theThing = this->addPixmap(pixmap);
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
