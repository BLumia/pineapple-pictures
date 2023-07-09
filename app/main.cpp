// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "mainwindow.h"

#include "playlistmanager.h"
#include "settings.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QTranslator>
#include <QUrl>

// QM_FILE_INSTALL_DIR should be defined from the CMakeLists file.
#ifndef QM_FILE_INSTALL_DIR
#define QM_FILE_INSTALL_DIR ":/i18n/"
#endif // QM_FILE_INSTALL_DIR

int main(int argc, char *argv[])
{
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Settings::instance()->hiDpiScaleFactorBehavior());

    QApplication a(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    a.setAttribute(Qt::ApplicationAttribute::AA_UseHighDpiPixmaps);
#endif

    QTranslator translator;
    QString qmDir;
#ifdef _WIN32
    qmDir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
#else
    qmDir = QT_STRINGIFY(QM_FILE_INSTALL_DIR);
#endif
    if (translator.load(QString("PineapplePictures_%1").arg(QLocale::system().name()), qmDir)) {
        a.installTranslator(&translator);
    }
    a.setApplicationName("Pineapple Pictures");
    a.setApplicationDisplayName(QCoreApplication::translate("main", "Pineapple Pictures"));

    // parse commandline arguments
    QCommandLineParser parser;
    parser.addPositionalArgument("File list", QCoreApplication::translate("main", "File list."));
    parser.addHelpOption();

    parser.process(a);

    MainWindow w;
    w.show();

    QStringList urlStrList = parser.positionalArguments();
    QList<QUrl> && urlList = PlaylistManager::convertToUrlList(urlStrList);

    if (!urlList.isEmpty()) {
        w.showUrls(urlList);
    }

    w.initWindowSize();

    return a.exec();
}
