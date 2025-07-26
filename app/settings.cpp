// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "settings.h"

#include <QApplication>
#include <QStandardPaths>
#include <QDebug>
#include <QDir>
#include <QAction>
#include <QWidget>
#include <QKeySequence>
#include <QMetaEnum>

namespace QEnumHelper
{
    template <typename E>
    E fromString(const QString &text, const E defaultValue)
    {
        bool ok;
        E result = static_cast<E>(QMetaEnum::fromType<E>().keyToValue(text.toUtf8(), &ok));
        if (!ok) {
            return defaultValue;
        }
        return result;
    }

    template <typename E>
    QString toString(E value)
    {
        const int intValue = static_cast<int>(value);
        return QString::fromUtf8(QMetaEnum::fromType<E>().valueToKey(intValue));
    }
}

Settings *Settings::m_settings_instance = nullptr;

Settings *Settings::instance()
{
    if (!m_settings_instance) {
        m_settings_instance = new Settings;
    }

    return m_settings_instance;
}

bool Settings::stayOnTop() const
{
    return m_qsettings->value("stay_on_top", true).toBool();
}

bool Settings::useBuiltInCloseAnimation() const
{
    return m_qsettings->value("use_built_in_close_animation", true).toBool();
}

bool Settings::useLightCheckerboard() const
{
    return m_qsettings->value("use_light_checkerboard", false).toBool();
}

bool Settings::loopGallery() const
{
    return m_qsettings->value("loop_gallery", true).toBool();
}

bool Settings::autoLongImageMode() const
{
    return m_qsettings->value("auto_long_image_mode", true).toBool();
}

Settings::DoubleClickBehavior Settings::doubleClickBehavior() const
{
    QString result = m_qsettings->value("double_click_behavior", "Close").toString();

    return QEnumHelper::fromString<DoubleClickBehavior>(result, DoubleClickBehavior::Close);
}

Settings::MouseWheelBehavior Settings::mouseWheelBehavior() const
{
    QString result = m_qsettings->value("mouse_wheel_behavior", "Zoom").toString();

    return QEnumHelper::fromString<MouseWheelBehavior>(result, MouseWheelBehavior::Zoom);
}

Settings::WindowSizeBehavior Settings::initWindowSizeBehavior() const
{
    QString result = m_qsettings->value("init_window_size_behavior", "Auto").toString();

    return QEnumHelper::fromString<WindowSizeBehavior>(result, WindowSizeBehavior::Auto);
}

Qt::HighDpiScaleFactorRoundingPolicy Settings::hiDpiScaleFactorBehavior() const
{
    QString result = m_qsettings->value("hidpi_scale_factor_behavior", "PassThrough").toString();

    return QEnumHelper::fromString<Qt::HighDpiScaleFactorRoundingPolicy>(result, Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}

void Settings::setStayOnTop(bool on)
{
    m_qsettings->setValue("stay_on_top", on);
    m_qsettings->sync();
}

void Settings::setUseBuiltInCloseAnimation(bool on)
{
    m_qsettings->setValue("use_built_in_close_animation", on);
    m_qsettings->sync();
}

void Settings::setUseLightCheckerboard(bool light)
{
    m_qsettings->setValue("use_light_checkerboard", light);
    m_qsettings->sync();
}

void Settings::setLoopGallery(bool on)
{
    m_qsettings->setValue("loop_gallery", on);
    m_qsettings->sync();
}

void Settings::setAutoLongImageMode(bool on)
{
    m_qsettings->setValue("auto_long_image_mode", on);
    m_qsettings->sync();
}

void Settings::setDoubleClickBehavior(DoubleClickBehavior dcb)
{
    m_qsettings->setValue("double_click_behavior", QEnumHelper::toString(dcb));
    m_qsettings->sync();
}

void Settings::setMouseWheelBehavior(MouseWheelBehavior mwb)
{
    m_qsettings->setValue("mouse_wheel_behavior", QEnumHelper::toString(mwb));
    m_qsettings->sync();
}

void Settings::setInitWindowSizeBehavior(WindowSizeBehavior wsb)
{
    m_qsettings->setValue("init_window_size_behavior", QEnumHelper::toString(wsb));
    m_qsettings->sync();
}

void Settings::setHiDpiScaleFactorBehavior(Qt::HighDpiScaleFactorRoundingPolicy hidpi)
{
    m_qsettings->setValue("hidpi_scale_factor_behavior", QEnumHelper::toString(hidpi));
    m_qsettings->sync();
}

void Settings::applyUserShortcuts(QWidget *widget)
{
    m_qsettings->beginGroup("shortcuts");
    const QStringList shortcutNames = m_qsettings->allKeys();
    for (const QString & name : shortcutNames) {
        QList<QKeySequence> shortcuts = m_qsettings->value(name).value<QList<QKeySequence>>();
        setShortcutsForAction(widget, name, shortcuts, false);
    }
    m_qsettings->endGroup();
}

bool Settings::setShortcutsForAction(QWidget *widget, const QString &objectName,
                                     QList<QKeySequence> shortcuts, bool writeConfig)
{
    QAction * targetAction = nullptr;
    for (QAction * action : widget->actions()) {
        if (action->objectName() == objectName) {
            targetAction = action;
        } else {
            for (const QKeySequence & shortcut : std::as_const(shortcuts)) {
                if (action->shortcuts().contains(shortcut)) {
                    return false;
                }
            }
        }
    }

    if (targetAction) {
        targetAction->setShortcuts(shortcuts);
    }

    if (targetAction && writeConfig) {
        m_qsettings->beginGroup("shortcuts");
        m_qsettings->setValue(objectName, QVariant::fromValue(shortcuts));
        m_qsettings->endGroup();
        m_qsettings->sync();
    }

    return true;
}

#if defined(FLAG_PORTABLE_MODE_SUPPORT) && defined(Q_OS_WIN)
#include <windows.h>
// QCoreApplication::applicationDirPath() parses the "applicationDirPath" from arg0, which...
// 1. rely on a QApplication object instance
//    but we need to call QGuiApplication::setHighDpiScaleFactorRoundingPolicy() before QApplication get created
// 2. arg0 is NOT garanteed to be the path of execution
//    see also: https://stackoverflow.com/questions/383973/is-args0-guaranteed-to-be-the-path-of-execution
// This function is here mainly for #1.
QString getApplicationDirPath()
{
    WCHAR buffer[MAX_PATH];
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    QString appPath = QString::fromWCharArray(buffer);

    return appPath.left(appPath.lastIndexOf('\\'));
}
#endif // defined(FLAG_PORTABLE_MODE_SUPPORT) && defined(Q_OS_WIN)

Settings::Settings()
    : QObject(qApp)
{
    QString configPath;

#if defined(FLAG_PORTABLE_MODE_SUPPORT) && defined(Q_OS_WIN)
    QString portableConfigDirPath = QDir(getApplicationDirPath()).absoluteFilePath("data");
    QFileInfo portableConfigDirInfo(portableConfigDirPath);
    if (portableConfigDirInfo.exists() && portableConfigDirInfo.isDir() && portableConfigDirInfo.isWritable()) {
        // we can use it.
        configPath = portableConfigDirPath;
    }
#endif // defined(FLAG_PORTABLE_MODE_SUPPORT) && defined(Q_OS_WIN)

    if (configPath.isEmpty()) {
        // Should be %LOCALAPPDATA%\<APPNAME> under Windows, ~/.config/<APPNAME> under Linux.
        configPath = QStandardPaths::writableLocation(QStandardPaths::AppConfigLocation);
    }

    m_qsettings = new QSettings(QDir(configPath).absoluteFilePath("config.ini"), QSettings::IniFormat, this);

    qRegisterMetaType<QList<QKeySequence>>();
}

