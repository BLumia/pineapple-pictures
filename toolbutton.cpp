#include "toolbutton.h"

#include "opacityhelper.h"

#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

ToolButton::ToolButton(QWidget *parent)
    : QPushButton(parent)
    , m_opacityHelper(new OpacityHelper(this))
{
    setFlat(true);
    setFixedSize(50, 50);
    setStyleSheet("QPushButton {"
                  "background: transparent;"
                  "}"
                  "QPushButton:hover {"
                  "background: red;"
                  "}");
}

void ToolButton::setOpacity(qreal opacity, bool animated)
{
    m_opacityHelper->setOpacity(opacity, animated);
}
