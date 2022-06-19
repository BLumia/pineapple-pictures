// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "opacityhelper.h"

#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

OpacityHelper::OpacityHelper(QWidget *parent)
    : QObject(parent)
    , m_opacityFx(new QGraphicsOpacityEffect(parent))
    , m_opacityAnimation(new QPropertyAnimation(m_opacityFx, "opacity"))
{
    parent->setGraphicsEffect(m_opacityFx);

    m_opacityAnimation->setDuration(300);
}

void OpacityHelper::setOpacity(qreal opacity, bool animated)
{
    if (!animated) {
        m_opacityFx->setOpacity(opacity);
        return;
    }

    m_opacityAnimation->stop();
    m_opacityAnimation->setStartValue(m_opacityFx->opacity());
    m_opacityAnimation->setEndValue(opacity);
    m_opacityAnimation->start();
}
