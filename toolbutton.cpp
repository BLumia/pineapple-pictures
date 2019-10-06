#include "toolbutton.h"

#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

ToolButton::ToolButton(QWidget *parent)
    : QPushButton(parent)
    , m_opacityFx(new QGraphicsOpacityEffect(this))
    , m_opacityAnimation(new QPropertyAnimation(m_opacityFx, "opacity"))
{
    setFlat(true);
    setFixedSize(50, 50);
    setGraphicsEffect(m_opacityFx);
    setStyleSheet("QPushButton {"
                  "background: transparent;"
                  "}"
                  "QPushButton:hover {"
                  "background: red;"
                  "}");

    m_opacityAnimation->setDuration(300);
}

void ToolButton::setIconOpacity(qreal opacity)
{
    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_opacityFx->opacity());
    m_opacityAnimation->setEndValue(opacity);
    m_opacityAnimation->start();
}
