#include "mainwindow.h"
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
    QApplication a(argc, argv);

    QTranslator translator;
    QString qmDir;
#ifdef _WIN32
    qmDir = QDir(QCoreApplication::applicationDirPath()).absoluteFilePath("translations");
#else
    qmDir = QT_STRINGIFY(QM_FILE_INSTALL_DIR);
#endif
    translator.load(QString("PineapplePictures_%1").arg(QLocale::system().name()), qmDir);
    a.installTranslator(&translator);
    a.setApplicationName("Pineapple Pictures");
    a.setApplicationDisplayName(QCoreApplication::translate("main", "Pineapple Pictures"));

    // parse commandline arguments
    QCommandLineParser parser;
    parser.addPositionalArgument("File list", QCoreApplication::translate("main", "File list."));
    parser.addHelpOption();

    parser.process(a);

    QStringList urlStrList = parser.positionalArguments();
    QList<QUrl> urlList;
    for (const QString & str : urlStrList) {
        QUrl url = QUrl::fromLocalFile(str);
        if (url.isValid()) {
            urlList.append(url);
        }
    }

    MainWindow w;
    w.show();

    if (!urlList.isEmpty()) {
        w.showUrls(urlList);
        w.adjustWindowSizeBySceneRect();
    }

    return a.exec();
}
