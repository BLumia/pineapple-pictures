#ifndef TOOLBUTTON_H
#define TOOLBUTTON_H

#include <QPushButton>

class OpacityHelper;
class ToolButton : public QPushButton
{
    Q_OBJECT
public:
    ToolButton(QWidget * parent = nullptr);

public slots:
    void setOpacity(qreal opacity, bool animated = true);

private:
    OpacityHelper * m_opacityHelper;
};

#endif // TOOLBUTTON_H
