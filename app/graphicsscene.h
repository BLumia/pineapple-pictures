#ifndef GRAPHICSSCENE_H
#define GRAPHICSSCENE_H

#include <QGraphicsScene>

class GraphicsScene : public QGraphicsScene
{
    Q_OBJECT
public:
    GraphicsScene(QObject *parent = nullptr);
    ~GraphicsScene();

    void showImage(const QPixmap &pixmap);
    void showText(const QString &text);
    void showSvg(const QString &filepath);
    void showAnimated(const QString &filepath);

    bool trySetTransformationMode(Qt::TransformationMode mode);

    QPixmap renderToPixmap();

private:
    QGraphicsItem * m_theThing;
};

#endif // GRAPHICSSCENE_H
