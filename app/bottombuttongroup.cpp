#include "bottombuttongroup.h"

#include "opacityhelper.h"

#include <functional>

#include <QToolButton>
#include <QVBoxLayout>
#include <QDebug>

BottomButtonGroup::BottomButtonGroup(const std::vector<QAction *> &actionList, QWidget *parent)
    : QGroupBox (parent)
    , m_opacityHelper(new OpacityHelper(this))
{
    QHBoxLayout * mainLayout = new QHBoxLayout(this);
    mainLayout->setSizeConstraint(QLayout::SetFixedSize);
    this->setLayout(mainLayout);
    this->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
    this->setStyleSheet("BottomButtonGroup {"
                        "border: 1px solid gray;"
                        "border-top-left-radius: 10px;"
                        "border-top-right-radius: 10px;"
                        "border-style: none;"
                        "background-color:rgba(0,0,0,120)"
                        "}"
                        "QToolButton {"
                        "background:transparent;"
                        "}");

    auto newActionBtn = [this](QAction * action) -> QToolButton * {
        QToolButton * btn = new QToolButton(this);
        btn->setDefaultAction(action);
        btn->setIconSize(QSize(40, 40));
        btn->setFixedSize(40, 40);
        return btn;
    };

    for (QAction * action : actionList) {
        addButton(newActionBtn(action));
    }
}

void BottomButtonGroup::setOpacity(qreal opacity, bool animated)
{
    m_opacityHelper->setOpacity(opacity, animated);
}

void BottomButtonGroup::addButton(QAbstractButton *button)
{
    layout()->addWidget(button);
    updateGeometry();
}
