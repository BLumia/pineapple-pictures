#include "bottombuttongroup.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

BottomButtonGroup::BottomButtonGroup(QWidget *parent)
    : QGroupBox (parent)
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
                        "QPushButton {"
                        "background-color:rgba(225,255,255,0);"
                        "color: white;"
                        "}");

    auto newBtn = [](QString text) -> QPushButton * {
        QPushButton * btn = new QPushButton(QIcon(QStringLiteral(":/icons/") + text), "");
        btn->setIconSize(QSize(40, 40));
        btn->setFixedSize(40, 40);
        return btn;
    };
    addButton(newBtn("zoom-original"));
    addButton(newBtn("view-fullscreen"));
    addButton(newBtn("zoom-in"));
    addButton(newBtn("zoom-out"));
    addButton(newBtn("object-rorate-right"));
}

void BottomButtonGroup::addButton(QAbstractButton *button)
{
    layout()->addWidget(button);
    updateGeometry();
}
