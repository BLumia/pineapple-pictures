#ifndef BOTTOMBUTTONGROUP_H
#define BOTTOMBUTTONGROUP_H

#include <QAbstractButton>
#include <QGroupBox>

class BottomButtonGroup : public QGroupBox
{
    Q_OBJECT
public:
    explicit BottomButtonGroup(QWidget *parent = nullptr);

    void addButton(QAbstractButton *button);

signals:
    void resetToOriginalBtnClicked();
    void zoomInBtnClicked();
    void zoomOutBtnClicked();
    void toggleCheckerboardBtnClicked();
};

#endif // BOTTOMBUTTONGROUP_H
