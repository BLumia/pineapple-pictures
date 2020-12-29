#include "framelesswindow.h"

#include <QVBoxLayout>

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout(this))
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    m_centralLayout->setMargin(0);
}

void FramelessWindow::setCentralWidget(QWidget *widget)
{
    if (m_centralWidget) {
        m_centralLayout->removeWidget(m_centralWidget);
        m_centralWidget->deleteLater();
    }

    m_centralLayout->addWidget(widget);
    m_centralWidget = widget;
}
