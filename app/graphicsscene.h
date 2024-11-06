// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

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

    bool trySetTransformationMode(Qt::TransformationMode mode, float scaleHint);

    bool togglePauseAnimation();
    bool skipAnimationFrame(int delta = 1);

    QPixmap renderToPixmap();

private:
    QGraphicsItem * m_theThing = nullptr;
};

#endif // GRAPHICSSCENE_H
