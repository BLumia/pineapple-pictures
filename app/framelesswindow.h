#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

class FramelessWindow : public QWidget
{
    Q_OBJECT
public:
    explicit FramelessWindow(QWidget *parent = nullptr);

    void setCentralWidget(QWidget * widget);

protected:
    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

private:
    QVBoxLayout * m_centralLayout = nullptr;
    QWidget * m_centralWidget = nullptr; // just a pointer, doesn't take the ownership.
};

#endif // FRAMELESSWINDOW_H
