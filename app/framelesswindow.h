// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef FRAMELESSWINDOW_H
#define FRAMELESSWINDOW_H

#include <QWidget>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    typedef qintptr NATIVE_RESULT;
#else
    typedef long NATIVE_RESULT;
#endif // QT_VERSION_CHECK(6, 0, 0)

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
    bool nativeEvent(const QByteArray& eventType, void* message, NATIVE_RESULT* result) override;

private:
    QVBoxLayout * m_centralLayout = nullptr;
    QWidget * m_centralWidget = nullptr; // just a pointer, doesn't take the ownership.
};

#endif // FRAMELESSWINDOW_H
