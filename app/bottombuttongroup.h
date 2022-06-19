// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef BOTTOMBUTTONGROUP_H
#define BOTTOMBUTTONGROUP_H

#include <vector>

#include <QAbstractButton>
#include <QGroupBox>

class OpacityHelper;
class BottomButtonGroup : public QGroupBox
{
    Q_OBJECT
public:
    explicit BottomButtonGroup(const std::vector<QAction *> & actionList, QWidget *parent = nullptr);

    void setOpacity(qreal opacity, bool animated = true);
    void addButton(QAbstractButton *button);

private:
    OpacityHelper * m_opacityHelper;
};

#endif // BOTTOMBUTTONGROUP_H
