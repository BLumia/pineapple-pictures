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
        FullScreen,
    };
    Q_ENUM(DoubleClickBehavior)

    enum MouseWheelBehavior {
        Zoom,
        Switch,
    };
    Q_ENUM(MouseWheelBehavior)

    enum WindowSizeBehavior {
        Auto,
        Maximized,
        Windowed,
    };
    Q_ENUM(WindowSizeBehavior)

    static Settings *instance();

    bool stayOnTop() const;
    bool useBuiltInCloseAnimation() const;
    bool useLightCheckerboard() const;
    bool loopGallery() const;
    bool autoLongImageMode() const;
    bool svgTiny12Only() const;
    DoubleClickBehavior doubleClickBehavior() const;
    MouseWheelBehavior mouseWheelBehavior() const;
    WindowSizeBehavior initWindowSizeBehavior() const;
    Qt::HighDpiScaleFactorRoundingPolicy hiDpiScaleFactorBehavior() const;

    void setStayOnTop(bool on);
    void setUseBuiltInCloseAnimation(bool on);
    void setUseLightCheckerboard(bool light);
    void setLoopGallery(bool on);
    void setAutoLongImageMode(bool on);
    void setSvgTiny12Only(bool on);
    void setDoubleClickBehavior(DoubleClickBehavior dcb);
    void setMouseWheelBehavior(MouseWheelBehavior mwb);
    void setInitWindowSizeBehavior(WindowSizeBehavior wsb);
    void setHiDpiScaleFactorBehavior(Qt::HighDpiScaleFactorRoundingPolicy hidpi);

    void applyUserShortcuts(QWidget * widget);
    bool setShortcutsForAction(QWidget * widget, const QString & objectName,
                               QList<QKeySequence> shortcuts, bool writeConfig = true);

private:
    Settings();

    static Settings *m_settings_instance;

    QSettings *m_qsettings;

signals:

public slots:
};
