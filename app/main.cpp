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

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName("Pineapple Pictures");
    QCoreApplication::setApplicationVersion(PPIC_VERSION_STRING);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Settings::instance()->hiDpiScaleFactorBehavior());

    QApplication a(argc, argv);
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    a.setAttribute(Qt::ApplicationAttribute::AA_UseHighDpiPixmaps);
#endif

    QTranslator translator;
#if defined(TRANSLATION_RESOURCE_EMBEDDING)
    const QString qmDir = QLatin1String(":/i18n/");
#elif defined(QM_FILE_INSTALL_ABSOLUTE_DIR)
    const QString qmDir = QT_STRINGIFY(QM_FILE_INSTALL_ABSOLUTE_DIR);
#else
    const QString qmDir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
#endif
    if (translator.load(QLocale(), QLatin1String("PineapplePictures"), QLatin1String("_"), qmDir)) {
        QCoreApplication::installTranslator(&translator);
    }

    QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Pineapple Pictures"));

    // commandline options
    QCommandLineOption supportedImageFormats(QStringLiteral("supported-image-formats"), QCoreApplication::translate("main", "List supported image format suffixes, and quit program."));
    // parse commandline arguments
    QCommandLineParser parser;
    parser.addOption(supportedImageFormats);
    parser.addPositionalArgument("File list", QCoreApplication::translate("main", "File list."));
    parser.addHelpOption();
    parser.process(a);

    if (parser.isSet(supportedImageFormats)) {
        fputs(qPrintable(MainWindow::supportedImageFormats().join(QChar('\n'))), stdout);
        ::exit(EXIT_SUCCESS);
    }

    MainWindow w;
    w.show();

    QStringList urlStrList = parser.positionalArguments();
    QList<QUrl> && urlList = PlaylistManager::convertToUrlList(urlStrList);

    if (!urlList.isEmpty()) {
        w.showUrls(urlList);
    }

    w.initWindowSize();

    return QApplication::exec();
}
