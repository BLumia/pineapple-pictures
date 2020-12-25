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
    app/aboutdialog.cpp \
    app/main.cpp \
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
    app/exiv2wrapper.cpp

HEADERS += \
    app/aboutdialog.h \
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
    app/exiv2wrapper.h

TRANSLATIONS = \
    translations/PineapplePictures.ts \
    translations/PineapplePictures_zh_CN.ts \
    translations/PineapplePictures_de.ts \
    translations/PineapplePictures_fr.ts \
    translations/PineapplePictures_nb_NO.ts \
    translations/PineapplePictures_ru.ts

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    assets/resources.qrc

# Generate from svg:
# magick convert -background none app-icon.svg -define icon:auto-resize="16,32,48,64,128,256" app-icon.ico
RC_ICONS = assets/icons/app-icon.ico

# Windows only, for rc file (we're not going to use the .rc file in this repo)
QMAKE_TARGET_PRODUCT = Pineapple Pictures
QMAKE_TARGET_DESCRIPTION = Pineapple Pictures - Image Viewer
QMAKE_TARGET_COPYRIGHT = MIT/Expat License - Copyright (C) 2020 Gary Wang
