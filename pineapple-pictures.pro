QT       += core widgets gui svg

TARGET = ppic
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11 lrelease embed_translations

SOURCES += \
    aboutdialog.cpp \
    main.cpp \
    mainwindow.cpp \
    graphicsview.cpp \
    bottombuttongroup.cpp \
    graphicsscene.cpp \
    navigatorview.cpp \
    opacityhelper.cpp \
    toolbutton.cpp \
    settings.cpp \
    settingsdialog.cpp \
    metadatamodel.cpp \
    metadatadialog.cpp

HEADERS += \
    aboutdialog.h \
    mainwindow.h \
    graphicsview.h \
    bottombuttongroup.h \
    graphicsscene.h \
    navigatorview.h \
    opacityhelper.h \
    toolbutton.h \
    settings.h \
    settingsdialog.h \
    metadatamodel.h \
    metadatadialog.h

TRANSLATIONS = \
    languages/PineapplePictures.ts \
    languages/PineapplePictures_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

# Generate from svg:
# magick convert -background none app-icon.svg -define icon:auto-resize="16,32,48,64,128,256" app-icon.ico
RC_ICONS = icons/app-icon.ico

# Windows only, for rc file (we're not going to use the .rc file in this repo)
QMAKE_TARGET_PRODUCT = Pineapple Pictures
QMAKE_TARGET_DESCRIPTION = Pineapple Pictures - Image Viewer
QMAKE_TARGET_COPYRIGHT = MIT/Expat License - Copyright (C) 2020 Gary Wang
