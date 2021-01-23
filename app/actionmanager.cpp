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
    CREATE_NEW_ACTION(mainWindow, actionCopyPixmap);
    CREATE_NEW_ACTION(mainWindow, actionCopyFilePath);
    CREATE_NEW_ACTION(mainWindow, actionPaste);
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

    actionCopyPixmap->setText(QCoreApplication::translate("MainWindow", "Copy P&ixmap", nullptr));
    actionCopyFilePath->setText(QCoreApplication::translate("MainWindow", "Copy &File Path", nullptr));
    actionPaste->setText(QCoreApplication::translate("MainWindow", "&Paste", nullptr));
    actionToggleStayOnTop->setText(QCoreApplication::translate("MainWindow", "Stay on top", nullptr));
    actionToggleProtectMode->setText(QCoreApplication::translate("MainWindow", "Protected mode", nullptr));
    actionSettings->setText(QCoreApplication::translate("MainWindow", "Configure...", nullptr));
    actionHelp->setText(QCoreApplication::translate("MainWindow", "Help", nullptr));
    actionProperties->setText(QCoreApplication::translate("MainWindow", "Properties", nullptr));
    actionQuitApp->setText(QCoreApplication::translate("MainWindow", "Quit", nullptr));
}

void ActionManager::setupShortcuts()
{
    actionQuitApp->setShortcuts({
        QKeySequence(Qt::Key_Space),
        QKeySequence(Qt::Key_Escape)
    });
    actionQuitApp->setShortcutContext(Qt::ApplicationShortcut);
}

