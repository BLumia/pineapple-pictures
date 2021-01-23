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

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings *instance();

    bool stayOnTop();
    DoubleClickBehavior doubleClickBehavior();
    MouseWheelBehavior mouseWheelBehavior();

    void setStayOnTop(bool on);
    void setDoubleClickBehavior(DoubleClickBehavior dcb);
    void setMouseWheelBehavior(MouseWheelBehavior mwb);

    static QString doubleClickBehaviorToString(DoubleClickBehavior dcb);
    static QString mouseWheelBehaviorToString(MouseWheelBehavior mwb);
    static DoubleClickBehavior stringToDoubleClickBehavior(QString str);
    static MouseWheelBehavior stringToMouseWheelBehavior(QString str);

private:
    Settings();

    static Settings *m_settings_instance;

    QSettings *m_qsettings;

signals:

public slots:
};

