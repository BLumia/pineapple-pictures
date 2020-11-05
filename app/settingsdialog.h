#ifndef SETTINGSDIALOG_H
#define SETTINGSDIALOG_H

#include <QObject>
#include <QDialog>

class QCheckBox;
class QComboBox;
class SettingsDialog : public QDialog
{
    Q_OBJECT
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

signals:

public slots:

private:
    QCheckBox * m_stayOnTop = nullptr;
    QComboBox * m_doubleClickBehavior = nullptr;
};

#endif // SETTINGSDIALOG_H
