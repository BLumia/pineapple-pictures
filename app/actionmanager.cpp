// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "actionmanager.h"

#include "mainwindow.h"

#include <QGuiApplication>
#include <QSvgRenderer>
#include <QPainter>

#define ICON_NAME(name)\
    QStringLiteral(":/icons/" #name ".svg")

#define ACTION_NAME(s) QStringLiteral(STRIFY(s))
#define STRIFY(s) #s

ActionManager::ActionManager()
{

}

ActionManager::~ActionManager()
{

}

QIcon ActionManager::loadHidpiIcon(const QString &resp, QSize sz)
{
    QSvgRenderer r(resp);
    QPixmap pm = QPixmap(sz * qApp->devicePixelRatio());
    pm.fill(Qt::transparent);
    QPainter p(&pm);
    r.render(&p);
    pm.setDevicePixelRatio(qApp->devicePixelRatio());
    return QIcon(pm);
}

void ActionManager::setupAction(MainWindow *mainWindow)
{
    auto create_action = [] (QWidget *w, QAction **a, QString i, QString an) {
        *a = new QAction(w);
        if (!i.isNull())
            (*a)->setIcon(ActionManager::loadHidpiIcon(i));
        (*a)->setObjectName(an);
        w->addAction(*a);
    };
    #define CREATE_NEW_ICON_ACTION(w, a, i) create_action(w, &a, ICON_NAME(i), ACTION_NAME(a))
    CREATE_NEW_ICON_ACTION(mainWindow, actionActualSize, zoom-original);
    CREATE_NEW_ICON_ACTION(mainWindow, actionToggleMaximize, view-fullscreen);
    CREATE_NEW_ICON_ACTION(mainWindow, actionZoomIn, zoom-in);
    CREATE_NEW_ICON_ACTION(mainWindow, actionZoomOut, zoom-out);
    CREATE_NEW_ICON_ACTION(mainWindow, actionToggleCheckerboard, view-background-checkerboard);
    CREATE_NEW_ICON_ACTION(mainWindow, actionRotateClockwise, object-rotate-right);
    #undef CREATE_NEW_ICON_ACTION

    #define CREATE_NEW_ACTION(w, a) create_action(w, &a, QString(), ACTION_NAME(a))
    CREATE_NEW_ACTION(mainWindow, actionPrevPicture);
    CREATE_NEW_ACTION(mainWindow, actionNextPicture);

    CREATE_NEW_ACTION(mainWindow, actionOpen);
    CREATE_NEW_ACTION(mainWindow, actionHorizontalFlip);
    CREATE_NEW_ACTION(mainWindow, actionFitInView);
    CREATE_NEW_ACTION(mainWindow, actionFitByWidth);
    CREATE_NEW_ACTION(mainWindow, actionCopyPixmap);
    CREATE_NEW_ACTION(mainWindow, actionCopyFilePath);
    CREATE_NEW_ACTION(mainWindow, actionPaste);
    CREATE_NEW_ACTION(mainWindow, actionToggleStayOnTop);
    CREATE_NEW_ACTION(mainWindow, actionToggleProtectMode);
    CREATE_NEW_ACTION(mainWindow, actionSettings);
    CREATE_NEW_ACTION(mainWindow, actionHelp);
    CREATE_NEW_ACTION(mainWindow, actionLocateInFileManager);
    CREATE_NEW_ACTION(mainWindow, actionProperties);
    CREATE_NEW_ACTION(mainWindow, actionQuitApp);
    #undef CREATE_NEW_ACTION

    retranslateUi(mainWindow);

    QMetaObject::connectSlotsByName(mainWindow);
}

void ActionManager::retranslateUi(MainWindow *mainWindow)
{
    Q_UNUSED(mainWindow);

    actionOpen->setText(QCoreApplication::translate("MainWindow", "&Open...", nullptr));

    actionActualSize->setText(QCoreApplication::translate("MainWindow", "Actual size", nullptr));
    actionToggleMaximize->setText(QCoreApplication::translate("MainWindow", "Toggle maximize", nullptr));
    actionZoomIn->setText(QCoreApplication::translate("MainWindow", "Zoom in", nullptr));
    actionZoomOut->setText(QCoreApplication::translate("MainWindow", "Zoom out", nullptr));
    actionToggleCheckerboard->setText(QCoreApplication::translate("MainWindow", "Toggle Checkerboard", nullptr));
    actionRotateClockwise->setText(QCoreApplication::translate("MainWindow", "Rotate right", nullptr));

    actionPrevPicture->setText(QCoreApplication::translate("MainWindow", "Previous image", nullptr));
    actionNextPicture->setText(QCoreApplication::translate("MainWindow", "Next image", nullptr));

    actionHorizontalFlip->setText(QCoreApplication::translate("MainWindow", "Flip &Horizontally", nullptr));
    actionFitInView->setText("Fit in view"); // TODO: what should it called?
    actionFitByWidth->setText("Fit by width"); // TODO: what should it called?
    actionCopyPixmap->setText(QCoreApplication::translate("MainWindow", "Copy P&ixmap", nullptr));
    actionCopyFilePath->setText(QCoreApplication::translate("MainWindow", "Copy &File Path", nullptr));
    actionPaste->setText(QCoreApplication::translate("MainWindow", "&Paste", nullptr));
    actionToggleStayOnTop->setText(QCoreApplication::translate("MainWindow", "Stay on top", nullptr));
    actionToggleProtectMode->setText(QCoreApplication::translate("MainWindow", "Protected mode", nullptr));
    actionSettings->setText(QCoreApplication::translate("MainWindow", "Configure...", nullptr));
    actionHelp->setText(QCoreApplication::translate("MainWindow", "Help", nullptr));
#ifdef Q_OS_WIN
    actionLocateInFileManager->setText(
        QCoreApplication::translate(
            "MainWindow", "Show in File Explorer",
            "File Explorer is the name of explorer.exe under Windows"
        )
    );
#else
    actionLocateInFileManager->setText(QCoreApplication::translate("MainWindow", "Show in directory", nullptr));
#endif // Q_OS_WIN
    actionProperties->setText(QCoreApplication::translate("MainWindow", "Properties", nullptr));
    actionQuitApp->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
}

void ActionManager::setupShortcuts()
{
    actionOpen->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_O));
    actionActualSize->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    actionZoomIn->setShortcut(QKeySequence(QKeySequence::ZoomIn));
    actionZoomOut->setShortcut(QKeySequence(QKeySequence::ZoomOut));
    actionPrevPicture->setShortcuts({
        QKeySequence(Qt::Key_PageUp),
        QKeySequence(Qt::Key_Left),
    });
    actionNextPicture->setShortcuts({
        QKeySequence(Qt::Key_PageDown),
        QKeySequence(Qt::Key_Right),
    });
    actionHorizontalFlip->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    actionCopyPixmap->setShortcut(QKeySequence(QKeySequence::Copy));
    actionPaste->setShortcut(QKeySequence::Paste);
    actionHelp->setShortcut(QKeySequence::HelpContents);
    actionSettings->setShortcut(QKeySequence::Preferences);
    actionProperties->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    actionQuitApp->setShortcuts({
        QKeySequence(Qt::Key_Space),
        QKeySequence(Qt::Key_Escape)
    });
}

