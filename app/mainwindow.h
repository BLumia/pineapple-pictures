// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "framelesswindow.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>

#if QT_VERSION >= QT_VERSION_CHECK(6, 0, 0)
    typedef QEnterEvent QT_ENTER_EVENT;
#else
    typedef QEvent QT_ENTER_EVENT;
#endif // QT_VERSION_CHECK(6, 0, 0)

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QGraphicsView;
QT_END_NAMESPACE

class ActionManager;
class PlaylistManager;
class ToolButton;
class GraphicsView;
class NavigatorView;
class BottomButtonGroup;
class MainWindow : public FramelessWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

    void showUrls(const QList<QUrl> &urls);
    void initWindowSize();
    void adjustWindowSizeBySceneRect();
    QUrl currentImageFileUrl() const;

    void clearGallery();
    void loadGalleryBySingleLocalFile(const QString &path);
    void galleryPrev();
    void galleryNext();

protected slots:
    void showEvent(QShowEvent *event) override;
    void enterEvent(QT_ENTER_EVENT *event) override;
    void leaveEvent(QEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseDoubleClickEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
    void contextMenuEvent(QContextMenuEvent *event) override;

    void centerWindow();
    void closeWindow();
    void updateWidgetsPosition();
    void toggleProtectedMode();
    void toggleStayOnTop();
    void toggleAvoidResetTransform();
    bool stayOnTop() const;
    bool canPaste() const;
    void quitAppAction(bool force = false);
    void toggleFullscreen();
    void toggleMaximize();

protected:
    QSize sizeHint() const override;

private slots:
    void on_actionOpen_triggered();

    void on_actionActualSize_triggered();
    void on_actionToggleMaximize_triggered();
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionToggleCheckerboard_triggered();
    void on_actionRotateClockwise_triggered();

    void on_actionPrevPicture_triggered();
    void on_actionNextPicture_triggered();

    void on_actionHorizontalFlip_triggered();
    void on_actionFitInView_triggered();
    void on_actionFitByWidth_triggered();
    void on_actionCopyPixmap_triggered();
    void on_actionCopyFilePath_triggered();
    void on_actionPaste_triggered();
    void on_actionToggleStayOnTop_triggered();
    void on_actionToggleProtectMode_triggered();
    void on_actionToggleAvoidResetTransform_triggered();
    void on_actionSettings_triggered();
    void on_actionHelp_triggered();
    void on_actionProperties_triggered();
    void on_actionLocateInFileManager_triggered();
    void on_actionQuitApp_triggered();

private:
    ActionManager *m_am;
    PlaylistManager *m_pm;

    QPoint m_oldMousePos;
    QPropertyAnimation *m_fadeOutAnimation;
    QPropertyAnimation *m_floatUpAnimation;
    QParallelAnimationGroup *m_exitAnimationGroup;
    ToolButton *m_closeButton;
    ToolButton *m_prevButton;
    ToolButton *m_nextButton;
    GraphicsView *m_graphicsView;
    NavigatorView *m_gv;
    BottomButtonGroup *m_bottomButtonGroup;
    bool m_protectedMode = false;
    bool m_clickedOnWindow = false;
};

#endif // MAINWINDOW_H
