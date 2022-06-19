// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#pragma once

#include <QObject>
#include <QSettings>

class Settings : public QObject
{
    Q_OBJECT
public:
    enum DoubleClickBehavior {
        Ignore,
        Close,
        Maximize,
    };
    Q_ENUM(DoubleClickBehavior);

    enum MouseWheelBehavior {
        Zoom,
        Switch,
    };
    Q_ENUM(MouseWheelBehavior);

    enum WindowSizeBehavior {
        Auto,
        Maximized,
    };
    Q_ENUM(WindowSizeBehavior);

    static Settings *instance();

    bool stayOnTop();
    DoubleClickBehavior doubleClickBehavior() const;
    MouseWheelBehavior mouseWheelBehavior() const;
    WindowSizeBehavior initWindowSizeBehavior() const;

    void setStayOnTop(bool on);
    void setDoubleClickBehavior(DoubleClickBehavior dcb);
    void setMouseWheelBehavior(MouseWheelBehavior mwb);
    void setInitWindowSizeBehavior(WindowSizeBehavior wsb);

private:
    Settings();

    static Settings *m_settings_instance;

    QSettings *m_qsettings;

signals:

public slots:
};

