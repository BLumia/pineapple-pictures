// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "mainwindow.h"

#include "playlistmanager.h"
#include "settings.h"

#ifdef Q_OS_MACOS
#include "fileopeneventhandler.h"
#endif // Q_OS_MACOS

#include <QApplication>
#include <QCommandLineParser>
#include <QDir>
#include <QTranslator>
#include <QUrl>

using namespace Qt::Literals::StringLiterals;

int main(int argc, char *argv[])
{
    QCoreApplication::setApplicationName(u"Pineapple Pictures"_s);
    QCoreApplication::setApplicationVersion(PPIC_VERSION_STRING);
    QGuiApplication::setHighDpiScaleFactorRoundingPolicy(Settings::instance()->hiDpiScaleFactorBehavior());

    QApplication a(argc, argv);

    QTranslator translator;
#if defined(TRANSLATION_RESOURCE_EMBEDDING)
    const QString qmDir = u":/i18n/"_s;
#elif defined(QM_FILE_INSTALL_ABSOLUTE_DIR)
    const QString qmDir = QT_STRINGIFY(QM_FILE_INSTALL_ABSOLUTE_DIR);
#else
    const QString qmDir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
#endif
    if (translator.load(QLocale(), u"PineapplePictures"_s, u"_"_s, qmDir)) {
        QCoreApplication::installTranslator(&translator);
    }

    QGuiApplication::setApplicationDisplayName(QCoreApplication::translate("main", "Pineapple Pictures"));

    // commandline options
    QCommandLineOption supportedImageFormats(u"supported-image-formats"_s, QCoreApplication::translate("main", "List supported image format suffixes, and quit program."));
    // parse commandline arguments
    QCommandLineParser parser;
    parser.addOption(supportedImageFormats);
    parser.addPositionalArgument("File list", QCoreApplication::translate("main", "File list."));
    parser.addHelpOption();
    parser.process(a);

    if (parser.isSet(supportedImageFormats)) {
#if QT_VERSION < QT_VERSION_CHECK(6, 9, 0)
        fputs(qPrintable(MainWindow::supportedImageFormats().join(QChar('\n'))), stdout);
        ::exit(EXIT_SUCCESS);
#else
        QCommandLineParser::showMessageAndExit(QCommandLineParser::MessageType::Information,
                                               MainWindow::supportedImageFormats().join(QChar('\n')));
#endif
    }

    MainWindow w;
    w.show();

#ifdef Q_OS_MACOS
    FileOpenEventHandler * fileOpenEventHandler = new FileOpenEventHandler(&a);
    a.installEventFilter(fileOpenEventHandler);
    a.connect(fileOpenEventHandler, &FileOpenEventHandler::fileOpen, [&w](const QUrl & url){
        if (w.isHidden()) {
            w.setWindowOpacity(1);
            w.showNormal();
        } else {
            w.activateWindow();
        }
        w.showUrls({url});
        w.initWindowSize();
    });
#endif // Q_OS_MACOS

    QStringList urlStrList = parser.positionalArguments();
    QList<QUrl> && urlList = PlaylistManager::convertToUrlList(urlStrList);

    if (!urlList.isEmpty()) {
        w.showUrls(urlList);
    }

    w.initWindowSize();

    return QApplication::exec();
}
