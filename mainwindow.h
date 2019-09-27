#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

protected slots:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

    void closeWindow();

private:
    QPoint m_oldMousePos;
    QPropertyAnimation *m_fadeOutAnimation;
    QPropertyAnimation *m_floatUpAnimation;
    QParallelAnimationGroup *m_exitAnimationGroup;
    QPushButton *m_closeButton;
    bool m_clickedOnWindow = false;
};

#endif // MAINWINDOW_H
