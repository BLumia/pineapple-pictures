#ifndef NAVIGATORVIEW_H
#define NAVIGATORVIEW_H

#include <QGraphicsView>

class NavigatorView : public QGraphicsView
{
    Q_OBJECT
public:
    NavigatorView(QWidget *parent = nullptr);

private:
    void wheelEvent(QWheelEvent *event) override;
};

#endif // NAVIGATORVIEW_H
