#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QPushButton>

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QPropertyAnimation;
QT_END_NAMESPACE

class ToolButton : public QPushButton
{
    Q_OBJECT
public:
    ToolButton(QWidget * parent = nullptr);

public slots:
    void setIconOpacity(qreal opacity);

private:
    QGraphicsOpacityEffect * m_opacityFx;
    QPropertyAnimation * m_opacityAnimation;
};

#endif // TOOLBUTTON_H
