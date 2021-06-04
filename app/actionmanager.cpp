#include "actionmanager.h"

#include "mainwindow.h"

#include <QCoreApplication>

#define CREATE_NEW_ACTION(window, action)\
    action = new QAction(window);\
    action->setObjectName(QString::fromUtf8( #action ));\
    window->addAction(action);

ActionManager::ActionManager()
{

}

ActionManager::~ActionManager()
{

}

void ActionManager::setupAction(MainWindow *mainWindow)
{
    CREATE_NEW_ACTION(mainWindow, actionZoomIn);
    CREATE_NEW_ACTION(mainWindow, actionZoomOut);
    CREATE_NEW_ACTION(mainWindow, actionHorizontalFlip);
    CREATE_NEW_ACTION(mainWindow, actionFitInView);
    CREATE_NEW_ACTION(mainWindow, actionFitByWidth);
    CREATE_NEW_ACTION(mainWindow, actionCopyPixmap);
    CREATE_NEW_ACTION(mainWindow, actionCopyFilePath);
    CREATE_NEW_ACTION(mainWindow, actionPaste);
    CREATE_NEW_ACTION(mainWindow, actionToggleCheckerboard);
    CREATE_NEW_ACTION(mainWindow, actionToggleStayOnTop);
    CREATE_NEW_ACTION(mainWindow, actionToggleProtectMode);
    CREATE_NEW_ACTION(mainWindow, actionSettings);
    CREATE_NEW_ACTION(mainWindow, actionHelp);
    CREATE_NEW_ACTION(mainWindow, actionProperties);
    CREATE_NEW_ACTION(mainWindow, actionQuitApp);

    retranslateUi(mainWindow);

    QMetaObject::connectSlotsByName(mainWindow);
}

void ActionManager::retranslateUi(MainWindow *mainWindow)
{
    Q_UNUSED(mainWindow);

    actionZoomIn->setText(QCoreApplication::translate("MainWindow", "Zoom in", nullptr));
    actionZoomOut->setText(QCoreApplication::translate("MainWindow", "Zoom out", nullptr));
    actionHorizontalFlip->setText(QCoreApplication::translate("MainWindow", "Flip &Horizontally", nullptr));
    actionFitInView->setText("Fit in view"); // TODO: what should it called?
    actionFitByWidth->setText("Fit by width"); // TODO: what should it called?
    actionCopyPixmap->setText(QCoreApplication::translate("MainWindow", "Copy P&ixmap", nullptr));
    actionCopyFilePath->setText(QCoreApplication::translate("MainWindow", "Copy &File Path", nullptr));
    actionPaste->setText(QCoreApplication::translate("MainWindow", "&Paste", nullptr));
    actionToggleCheckerboard->setText(QCoreApplication::translate("MainWindow", "Toggle Checkerboard", nullptr));
    actionToggleStayOnTop->setText(QCoreApplication::translate("MainWindow", "Stay on top", nullptr));
    actionToggleProtectMode->setText(QCoreApplication::translate("MainWindow", "Protected mode", nullptr));
    actionSettings->setText(QCoreApplication::translate("MainWindow", "Configure...", nullptr));
    actionHelp->setText(QCoreApplication::translate("MainWindow", "Help", nullptr));
    actionProperties->setText(QCoreApplication::translate("MainWindow", "Properties", nullptr));
    actionQuitApp->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
}

void ActionManager::setupShortcuts()
{
    actionZoomIn->setShortcut(QKeySequence(QKeySequence::ZoomIn));
    actionZoomOut->setShortcut(QKeySequence(QKeySequence::ZoomOut));
    actionHorizontalFlip->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_R));
    actionCopyPixmap->setShortcut(QKeySequence(QKeySequence::Copy));
    actionPaste->setShortcut(QKeySequence::Paste);
    actionHelp->setShortcut(QKeySequence::HelpContents);
    actionSettings->setShortcut(QKeySequence::Preferences);
    actionProperties->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_I));
    actionQuitApp->setShortcuts({
        QKeySequence(Qt::Key_Space),
        QKeySequence(Qt::Key_Escape)
    });
}

