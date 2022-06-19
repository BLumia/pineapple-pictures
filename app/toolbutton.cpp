// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "toolbutton.h"

#include "opacityhelper.h"

#include <QPainter>
#include <QGraphicsOpacityEffect>
#include <QPropertyAnimation>

ToolButton::ToolButton(bool hoverColor, QWidget *parent)
    : QPushButton(parent)
    , m_opacityHelper(new OpacityHelper(this))
{
    setFlat(true);
    QString qss = "QPushButton {"
                  "background: transparent;"
                  "}";
    if (hoverColor) {
        qss += "QPushButton:hover {"
               "background: red;"
               "}";
    }
    setStyleSheet(qss);
}

void ToolButton::setOpacity(qreal opacity, bool animated)
{
    m_opacityHelper->setOpacity(opacity, animated);
}
