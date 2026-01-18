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
    auto create_action = [] (QWidget *w, QAction **a, QString i, QString an, bool iconFromTheme = false) {
        *a = new QAction(w);
        if (!i.isNull())
            (*a)->setIcon(iconFromTheme ? QIcon::fromTheme(i) : ActionManager::loadHidpiIcon(i));
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
    #define CREATE_NEW_THEMEICON_ACTION(w, a, i) create_action(w, &a, QLatin1String(STRIFY(i)), ACTION_NAME(a), true)
    CREATE_NEW_ACTION(mainWindow, actionRotateCounterClockwise);
    CREATE_NEW_ACTION(mainWindow, actionPrevPicture);
    CREATE_NEW_ACTION(mainWindow, actionNextPicture);

    CREATE_NEW_ACTION(mainWindow, actionTogglePauseAnimation);
    CREATE_NEW_ACTION(mainWindow, actionAnimationNextFrame);

    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionOpen, document-open);
    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionSaveAs, document-save-as);
    CREATE_NEW_ACTION(mainWindow, actionHorizontalFlip);
    CREATE_NEW_ACTION(mainWindow, actionFitInView);
    CREATE_NEW_ACTION(mainWindow, actionFitByWidth);
    CREATE_NEW_ACTION(mainWindow, actionFitLongImage);
    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionCopyPixmap, edit-copy);
    CREATE_NEW_ACTION(mainWindow, actionCopyFilePath);
    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionPaste, edit-paste);
    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionTrash, edit-delete);
    CREATE_NEW_ACTION(mainWindow, actionToggleStayOnTop);
    CREATE_NEW_ACTION(mainWindow, actionToggleProtectMode);
    CREATE_NEW_ACTION(mainWindow, actionToggleAvoidResetTransform);
    CREATE_NEW_ACTION(mainWindow, actionSettings);
    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionHelp, system-help);
    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionLocateInFileManager, system-file-manager);
    CREATE_NEW_THEMEICON_ACTION(mainWindow, actionProperties, document-properties);
    CREATE_NEW_ACTION(mainWindow, actionQuitApp);
    #undef CREATE_NEW_ACTION
    #undef CREATE_NEW_THEMEICON_ACTION

    retranslateUi(mainWindow);

    QMetaObject::connectSlotsByName(mainWindow);
}

void ActionManager::retranslateUi(MainWindow *mainWindow)
{
    Q_UNUSED(mainWindow);

    actionOpen->setText(QCoreApplication::translate("MainWindow", "&Open...", nullptr));
    actionSaveAs->setText(QCoreApplication::translate("MainWindow", "Save &As...", nullptr));

    actionActualSize->setText(QCoreApplication::translate("MainWindow", "Actual size", nullptr));
    actionToggleMaximize->setText(QCoreApplication::translate("MainWindow", "Toggle maximize", nullptr));
    actionZoomIn->setText(QCoreApplication::translate("MainWindow", "Zoom in", nullptr));
    actionZoomOut->setText(QCoreApplication::translate("MainWindow", "Zoom out", nullptr));
    actionToggleCheckerboard->setText(QCoreApplication::translate("MainWindow", "Toggle Checkerboard", nullptr));
    actionRotateClockwise->setText(QCoreApplication::translate("MainWindow", "Rotate right", nullptr));
    actionRotateCounterClockwise->setText(QCoreApplication::translate("MainWindow", "Rotate left", nullptr));

    actionPrevPicture->setText(QCoreApplication::translate("MainWindow", "Previous image", nullptr));
    actionNextPicture->setText(QCoreApplication::translate("MainWindow", "Next image", nullptr));

    actionTogglePauseAnimation->setText(QCoreApplication::translate("MainWindow", "Pause/Resume Animation", nullptr));
    actionAnimationNextFrame->setText(QCoreApplication::translate("MainWindow", "Animation Go to Next Frame", nullptr));

    actionHorizontalFlip->setText(QCoreApplication::translate("MainWindow", "Flip &Horizontally", nullptr));
    actionFitInView->setText(QCoreApplication::translate("MainWindow", "Fit to view", nullptr));
    actionFitByWidth->setText(QCoreApplication::translate("MainWindow", "Fit to width", nullptr));
    actionFitLongImage->setText(QCoreApplication::translate("MainWindow", "Fit long image", nullptr));
    actionCopyPixmap->setText(QCoreApplication::translate("MainWindow", "Copy P&ixmap", nullptr));
    actionCopyFilePath->setText(QCoreApplication::translate("MainWindow", "Copy &File Path", nullptr));
    actionPaste->setText(QCoreApplication::translate("MainWindow", "&Paste", nullptr));
    actionTrash->setText(QCoreApplication::translate("MainWindow", "Move to Trash", nullptr));
    actionToggleStayOnTop->setText(QCoreApplication::translate("MainWindow", "Stay on top", nullptr));
    actionToggleProtectMode->setText(QCoreApplication::translate("MainWindow", "Protected mode", nullptr));
    actionToggleAvoidResetTransform->setText(QCoreApplication::translate("MainWindow", "Keep transformation", "The 'transformation' means the flip/rotation status that currently applied to the image view"));
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
    actionOpen->setShortcut(QKeySequence::Open);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    actionActualSize->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_0));
    actionZoomIn->setShortcut(QKeySequence::ZoomIn);
    actionZoomOut->setShortcut(QKeySequence::ZoomOut);
    actionPrevPicture->setShortcuts({
        QKeySequence(Qt::Key_PageUp),
        QKeySequence(Qt::Key_Left),
    });
    actionNextPicture->setShortcuts({
        QKeySequence(Qt::Key_PageDown),
        QKeySequence(Qt::Key_Right),
    });
    actionHorizontalFlip->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_R));
    actionCopyPixmap->setShortcut(QKeySequence::Copy);
    actionPaste->setShortcut(QKeySequence::Paste);
    actionTrash->setShortcut(QKeySequence::Delete);
    actionHelp->setShortcut(QKeySequence::HelpContents);
    actionSettings->setShortcut(QKeySequence::Preferences);
    actionProperties->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_I));
    actionQuitApp->setShortcuts({
        QKeySequence(Qt::Key_Space),
        QKeySequence(Qt::Key_Escape)
    });
}

