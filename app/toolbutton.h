// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QPushButton>

class OpacityHelper;
class ToolButton : public QPushButton
{
    Q_OBJECT
public:
    ToolButton(bool hoverColor = false, QWidget * parent = nullptr);
    void setIconResourcePath(const QString &iconp);

public slots:
    void setOpacity(qreal opacity, bool animated = true);

private:
    OpacityHelper * m_opacityHelper;
};

#endif // TOOLBUTTON_H
