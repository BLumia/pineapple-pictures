#ifndef OPACITYHELPER_H
#define OPACITYHELPER_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QPropertyAnimation;
QT_END_NAMESPACE

class OpacityHelper : QObject
{
public:
    OpacityHelper(QWidget * parent);

    void setOpacity(qreal opacity, bool animated = true);

protected:
    QGraphicsOpacityEffect * m_opacityFx;
    QPropertyAnimation * m_opacityAnimation;
};

#endif // OPACITYHELPER_H
