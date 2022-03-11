#pragma once

#include <QObject>
#include <QSettings>

enum DoubleClickBehavior {
    ActionDoNothing,
    ActionCloseWindow,
    ActionMaximizeWindow,

    DCActionStart = ActionDoNothing,
    DCActionEnd = ActionMaximizeWindow
};

enum MouseWheelBehavior {
    ActionZoomImage,
    ActionPrevNextImage,

    MWActionStart = ActionZoomImage,
    MWActionEnd = ActionPrevNextImage
};

enum WindowSizeBehavior {
    ActionAutoSize,
    ActionMaximize,

    IWSActionStart = ActionAutoSize,
    IWSActionEnd = ActionMaximize
};

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings *instance();

    bool stayOnTop();
    DoubleClickBehavior doubleClickBehavior() const;
    MouseWheelBehavior mouseWheelBehavior() const;
    WindowSizeBehavior initWindowSizeBehavior() const;

    void setStayOnTop(bool on);
    void setDoubleClickBehavior(DoubleClickBehavior dcb);
    void setMouseWheelBehavior(MouseWheelBehavior mwb);
    void setInitWindowSizeBehavior(WindowSizeBehavior wsb);

    static QString doubleClickBehaviorToString(DoubleClickBehavior dcb);
    static QString mouseWheelBehaviorToString(MouseWheelBehavior mwb);
    static QString windowSizeBehaviorToString(WindowSizeBehavior wsb);
    static DoubleClickBehavior stringToDoubleClickBehavior(QString str);
    static MouseWheelBehavior stringToMouseWheelBehavior(QString str);
    static WindowSizeBehavior stringToWindowSizeBehavior(QString str);

private:
    Settings();

    static Settings *m_settings_instance;

    QSettings *m_qsettings;

signals:

public slots:
};

