// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

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
    QCheckBox * m_useBuiltInCloseAnimation = nullptr;
    QCheckBox * m_useLightCheckerboard = nullptr;
    QComboBox * m_doubleClickBehavior = nullptr;
    QComboBox * m_mouseWheelBehavior = nullptr;
    QComboBox * m_initWindowSizeBehavior = nullptr;
    QComboBox * m_hiDpiRoundingPolicyBehavior = nullptr;
};

#endif // SETTINGSDIALOG_H
