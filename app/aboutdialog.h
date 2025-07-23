// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef ABOUTDIALOG_H
#define ABOUTDIALOG_H

#include <QDialog>

QT_BEGIN_NAMESPACE
class QTextBrowser;
class QTabWidget;
class QDialogButtonBox;
QT_END_NAMESPACE

class AboutDialog : public QDialog
{
    Q_OBJECT
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog() override;

    QSize sizeHint() const override;

private:
    QTabWidget * m_tabWidget = nullptr;
    QDialogButtonBox * m_buttonBox = nullptr;

    QTextBrowser * m_helpTextEdit = nullptr;
    QTextBrowser * m_aboutTextEdit = nullptr;
    QTextBrowser * m_specialThanksTextEdit = nullptr;
    QTextBrowser * m_licenseTextEdit = nullptr;
    QTextBrowser * m_3rdPartyLibsTextEdit = nullptr;
};

#endif // ABOUTDIALOG_H
