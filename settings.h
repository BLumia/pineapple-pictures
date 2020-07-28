#pragma once

#include <QObject>
#include <QSettings>

enum DoubleClickBehavior {
    ActionDoNothing,
    ActionCloseWindow,
    ActionMaximizeWindow,

    ActionStart = ActionDoNothing,
    ActionEnd = ActionMaximizeWindow
};

class Settings : public QObject
{
    Q_OBJECT
public:
    static Settings *instance();

    bool stayOnTop();
    DoubleClickBehavior doubleClickBehavior();

    void setStayOnTop(bool on);
    void setDoubleClickBehavior(DoubleClickBehavior dcb);

    static QString doubleClickBehaviorToString(DoubleClickBehavior dcb);
    static DoubleClickBehavior stringToDoubleClickBehavior(QString str);

private:
    Settings();

    static Settings *m_settings_instance;

    QSettings *m_qsettings;

signals:

public slots:
};

