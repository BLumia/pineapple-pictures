#include "mainwindow.h"

#include "graphicsview.h"

#include <QMouseEvent>
#include <QMovie>
#include <QDebug>
#include <QGraphicsTextItem>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setMinimumSize(610, 410);

    m_fadeOutAnimation = new QPropertyAnimation(this, "windowOpacity");
    m_fadeOutAnimation->setDuration(300);
    m_fadeOutAnimation->setStartValue(1);
    m_fadeOutAnimation->setEndValue(0);
    m_floatUpAnimation = new QPropertyAnimation(this, "geometry");
    m_floatUpAnimation->setDuration(300);
    m_floatUpAnimation->setEasingCurve(QEasingCurve::OutCirc);
    m_exitAnimationGroup = new QParallelAnimationGroup;
    m_exitAnimationGroup->addAnimation(m_fadeOutAnimation);
    m_exitAnimationGroup->addAnimation(m_floatUpAnimation);
    connect(m_exitAnimationGroup, &QParallelAnimationGroup::finished,
            this, &QMainWindow::close);

    QGraphicsScene * scene = new QGraphicsScene(this);
    QGraphicsTextItem * textItem = scene->addText("Hello, world!");
    textItem->setDefaultTextColor(QColor("White"));

    GraphicsView * test = new GraphicsView(this);
    test->setScene(scene);
    this->setCentralWidget(test);

    m_closeButton = new QPushButton(test);
    m_closeButton->setFlat(true);
    m_closeButton->setFixedSize(50, 50);
    m_closeButton->setStyleSheet("QPushButton {"
                                 "background: transparent;"
                                 "}"
                                 "QPushButton:hover {"
                                 "background: red;"
                                 "}");

    connect(m_closeButton, &QAbstractButton::clicked,
            this, &MainWindow::closeWindow);
}

MainWindow::~MainWindow()
{

}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton) {
        m_clickedOnWindow = true;
        m_oldMousePos = event->pos();
        qDebug() << m_oldMousePos;
        event->accept();
    }

    return QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_clickedOnWindow) {
        move(event->globalPos() - m_oldMousePos);
        event->accept();
    }

    return QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_clickedOnWindow = false;

    return QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    closeWindow();

    return QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    m_closeButton->move(width() - m_closeButton->width(), 0);

    return QMainWindow::resizeEvent(event);
}

void MainWindow::closeWindow()
{
    QRect windowRect(this->geometry());
    m_floatUpAnimation->setStartValue(windowRect);
    m_floatUpAnimation->setEndValue(windowRect.adjusted(0, -80, 0, 0));
    m_floatUpAnimation->setStartValue(QRect(this->geometry().x(), this->geometry().y(), this->geometry().width(), this->geometry().height()));
    m_floatUpAnimation->setEndValue(QRect(this->geometry().x(), this->geometry().y()-80, this->geometry().width(), this->geometry().height()));
    m_exitAnimationGroup->start();
}
