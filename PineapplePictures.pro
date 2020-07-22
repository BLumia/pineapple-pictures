#-------------------------------------------------
#
# Project created by QtCreator 2019-09-26T23:36:07
#
#-------------------------------------------------

QT       += core gui svg

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = PineapplePictures
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
        main.cpp \
        mainwindow.cpp \
    graphicsview.cpp \
    bottombuttongroup.cpp \
    graphicsscene.cpp \
    navigatorview.cpp \
    opacityhelper.cpp \
    toolbutton.cpp

HEADERS += \
        mainwindow.h \
    graphicsview.h \
    bottombuttongroup.h \
    graphicsscene.h \
    navigatorview.h \
    opacityhelper.h \
    toolbutton.h

TRANSLATIONS = \
    languages/PineapplePictures.ts \
    languages/PineapplePictures_zh_CN.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    resources.qrc

# Generate fron svg:
# magick convert -background none app-icon.svg -define icon:auto-resize="16,32,48,64,128,256" app-icon.ico
RC_ICONS = icons/app-icon.ico
