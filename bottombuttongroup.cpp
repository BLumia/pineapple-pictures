#include "bottombuttongroup.h"

#include "opacityhelper.h"

#include <functional>

#include <QPushButton>
#include <QVBoxLayout>
#include <QDebug>

BottomButtonGroup::BottomButtonGroup(QWidget *parent)
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
                        "QPushButton {"
                        "background-color:rgba(225,255,255,0);"
                        "color: white;"
                        "border-style: none;"
                        "}");

    auto newBtn = [](QString text, std::function<void()> func) -> QPushButton * {
        QPushButton * btn = new QPushButton(QIcon(QStringLiteral(":/icons/") + text), "");
        btn->setIconSize(QSize(40, 40));
        btn->setFixedSize(40, 40);
        connect(btn, &QAbstractButton::clicked, btn, func);
        return btn;
    };
    addButton(newBtn("zoom-original", [this]() {
        emit resetToOriginalBtnClicked();
    }));
    addButton(newBtn("view-fullscreen", [this]() {
        emit toggleWindowMaximum();
    }));
    addButton(newBtn("zoom-in", [this]() {
        emit zoomInBtnClicked();
    }));
    addButton(newBtn("zoom-out", [this]() {
        emit zoomOutBtnClicked();
    }));
    addButton(newBtn("view-background-checkerboard", [this]() {
        emit toggleCheckerboardBtnClicked();
    }));
    addButton(newBtn("object-rotate-right", [this]() {
        emit rotateRightBtnClicked();
    }));
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
