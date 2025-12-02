# SPDX-FileCopyrightText: 2025 Gary Wang <git@blumia.net>
#
# SPDX-License-Identifier: MIT

QT       += core widgets gui svg svgwidgets

TARGET = ppic
TEMPLATE = app
DEFINES += PPIC_VERSION_STRING=\\\"x.y.z\\\"

win32 {
    DEFINES += FLAG_PORTABLE_MODE_SUPPORT=1
}

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++17 lrelease embed_translations

SOURCES += \
    app/aboutdialog.cpp \
    app/main.cpp \
    app/framelesswindow.cpp \
    app/mainwindow.cpp \
    app/graphicsview.cpp \
    app/bottombuttongroup.cpp \
    app/graphicsscene.cpp \
    app/navigatorview.cpp \
    app/opacityhelper.cpp \
    app/toolbutton.cpp \
    app/settings.cpp \
    app/settingsdialog.cpp \
    app/metadatamodel.cpp \
    app/metadatadialog.cpp \
    app/exiv2wrapper.cpp \
    app/actionmanager.cpp \
    app/playlistmanager.cpp \
    app/shortcutedit.cpp \
    app/fileopeneventhandler.cpp

HEADERS += \
    app/aboutdialog.h \
    app/framelesswindow.h \
    app/mainwindow.h \
    app/graphicsview.h \
    app/bottombuttongroup.h \
    app/graphicsscene.h \
    app/navigatorview.h \
    app/opacityhelper.h \
    app/toolbutton.h \
    app/settings.h \
    app/settingsdialog.h \
    app/metadatamodel.h \
    app/metadatadialog.h \
    app/exiv2wrapper.h \
    app/actionmanager.h \
    app/playlistmanager.h \
    app/shortcutedit.h \
    app/fileopeneventhandler.h

TRANSLATIONS = \
    app/translations/PineapplePictures_en.ts \
    app/translations/PineapplePictures_zh_CN.ts \
    app/translations/PineapplePictures_de.ts \
    app/translations/PineapplePictures_es.ts \
    app/translations/PineapplePictures_fr.ts \
    app/translations/PineapplePictures_nb_NO.ts \
    app/translations/PineapplePictures_nl.ts \
    app/translations/PineapplePictures_ru.ts \
    app/translations/PineapplePictures_si.ts \
    app/translations/PineapplePictures_id.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets/resources.qrc

# Generate from svg:
# magick -density 512x512 -background none app-icon.svg -define icon:auto-resize app-icon.ico
RC_ICONS = assets/icons/app-icon.ico

# Windows only, for rc file (we're not going to use the .rc file in this repo)
QMAKE_TARGET_PRODUCT = Pineapple Pictures
QMAKE_TARGET_DESCRIPTION = Pineapple Pictures - Image Viewer
QMAKE_TARGET_COPYRIGHT = MIT/Expat License - Copyright (C) 2024 Gary Wang

# MSVC only, since QMake doesn't have a CMAKE_CXX_STANDARD_LIBRARIES or cpp_winlibs similar thing
win32-msvc* {
    LIBS += -luser32
}
