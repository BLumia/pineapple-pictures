#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QAction>

class MainWindow;

class ActionManager
{
public:
    ActionManager();
    ~ActionManager();

    void setupAction(MainWindow * mainWindow);
    void retranslateUi(MainWindow *MainWindow);
    void setupShortcuts();

public:
    QAction *actionZoomIn;
    QAction *actionZoomOut;
    QAction *actionHorizontalFlip;
    QAction *actionFitByWidth;
    QAction *actionCopyPixmap;
    QAction *actionCopyFilePath;
    QAction *actionPaste;
    QAction *actionToggleCheckerboard;
    QAction *actionToggleStayOnTop;
    QAction *actionToggleProtectMode;
    QAction *actionSettings;
    QAction *actionHelp;
    QAction *actionProperties;
    QAction *actionQuitApp;
};

#endif // ACTIONMANAGER_H
