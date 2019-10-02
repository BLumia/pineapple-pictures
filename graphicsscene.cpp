#include "graphicsscene.h"

#include <QGraphicsSceneMouseEvent>
#include <QMimeData>
#include <QDebug>
#include <QGraphicsItem>
#include <QUrl>
#include <QGraphicsSvgItem>
#include <QMovie>
#include <QLabel>

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

void GraphicsScene::showGif(const QString &filepath)
{
    this->clear();
    QMovie * movie = new QMovie(filepath);
    QLabel * label = new QLabel;
    label->setStyleSheet("background-color:rgba(225,255,255,0);");
    label->setMovie(movie);
    this->addWidget(label);
    movie->start();
    m_theThing = this->addRect(QRect(QPoint(0, 0), label->sizeHint()),
                               QPen(Qt::transparent));
    this->setSceneRect(m_theThing->boundingRect());
}
