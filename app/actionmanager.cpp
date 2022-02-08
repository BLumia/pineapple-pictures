#include "actionmanager.h"

#include "mainwindow.h"

#include <QCoreApplication>

#define ICON_NAME(name)\
    QStringLiteral(":/icons/" #name "")

#define SETUP_NEW_ACTION(window, action)\
    action->setObjectName(QString::fromUtf8( #action ));\
    window->addAction(action);

#define CREATE_NEW_ACTION(window, action)\
    action = new QAction(window);\
    SETUP_NEW_ACTION(window, action)

#define CREATE_NEW_ICON_ACTION(window, action, iconname)\
    action = new QAction(QIcon(ICON_NAME(iconname)), QString(), window);\
    SETUP_NEW_ACTION(window, action)

ActionManager::ActionManager()
{

}

ActionManager::~ActionManager()
{

}

void ActionManager::setupAction(MainWindow *mainWindow)
{
    CREATE_NEW_ICON_ACTION(mainWindow, actionActualSize, zoom-original);
    CREATE_NEW_ICON_ACTION(mainWindow, actionToggleMaximize, view-fullscreen);
    CREATE_NEW_ICON_ACTION(mainWindow, actionZoomIn, zoom-in);
    CREATE_NEW_ICON_ACTION(mainWindow, actionZoomOut, zoom-out);
    CREATE_NEW_ICON_ACTION(mainWindow, actionToggleCheckerboard, view-background-checkerboard);
    CREATE_NEW_ICON_ACTION(mainWindow, actionRotateClockwise, object-rotate-right);

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

