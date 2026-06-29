// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "toolbutton.h"

#include "actionmanager.h"
#include "opacityhelper.h"

#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

ToolButton::ToolButton(QWidget *parent)
    : QPushButton(parent)
    , m_opacityHelper(new OpacityHelper(this))
{
    setFlat(true);
    QString qss = "QPushButton {"
                  "background: transparent;"
                  "}";
    setStyleSheet(qss);
}

void ToolButton::setIconResourcePath(const QString &iconp)
{
    this->setIcon(ActionManager::loadHidpiIcon(iconp, this->iconSize()));
}

void ToolButton::setOpacity(qreal opacity, bool animated)
{
    m_opacityHelper->setOpacity(opacity, animated);
}
