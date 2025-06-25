// SPDX-FileCopyrightText: 2025 Gary Wang <git@blumia.net>
//
// SPDX-License-Identifier: MIT

#ifndef ACTIONMANAGER_H
#define ACTIONMANAGER_H

#include <QAction>

class MainWindow;

class ActionManager
{
public:
    explicit ActionManager() = default;
    ~ActionManager() = default;

    void setupAction(MainWindow * mainWindow);
    void retranslateUi(MainWindow *MainWindow);
    void setupShortcuts();

    static QIcon loadHidpiIcon(const QString &resp, QSize sz = QSize(32, 32));

public:
    QAction *actionOpen;

    QAction *actionActualSize;
    QAction *actionToggleMaximize;
    QAction *actionZoomIn;
    QAction *actionZoomOut;
    QAction *actionToggleCheckerboard;
    QAction *actionRotateClockwise;
    QAction *actionRotateCounterClockwise;

    QAction *actionPrevPicture;
    QAction *actionNextPicture;

    QAction *actionTogglePauseAnimation;
    QAction *actionAnimationNextFrame;

    QAction *actionHorizontalFlip;
    QAction *actionFitInView;
    QAction *actionFitByWidth;
    QAction *actionCopyPixmap;
    QAction *actionCopyFilePath;
    QAction *actionPaste;
    QAction *actionTrash;
    QAction *actionToggleStayOnTop;
    QAction *actionToggleProtectMode;
    QAction *actionToggleAvoidResetTransform;
    QAction *actionSettings;
    QAction *actionHelp;
    QAction *actionLocateInFileManager;
    QAction *actionProperties;
    QAction *actionQuitApp;
};

#endif // ACTIONMANAGER_H
