#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "framelesswindow.h"

#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>

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
    void adjustWindowSizeBySceneRect();
    QUrl currentImageFileUrl() const;

    void clearGallery();
    void loadGalleryBySingleLocalFile(const QString &path);
    void galleryPrev();
    void galleryNext();

protected slots:
    void showEvent(QShowEvent *event) override;
    void enterEvent(QEvent *event) override;
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
    void toggleCheckerboard();
    void toggleProtectedMode();
    void toggleStayOnTop();
    bool stayOnTop();
    bool canPaste();
    void quitAppAction(bool force = false);
    void toggleFullscreen();
    void toggleMaximize();

protected:
    QSize sizeHint() const override;

private slots:
    void on_actionZoomIn_triggered();
    void on_actionZoomOut_triggered();
    void on_actionHorizontalFlip_triggered();
    void on_actionFitByWidth_triggered();
    void on_actionCopyPixmap_triggered();
    void on_actionCopyFilePath_triggered();
    void on_actionPaste_triggered();
    void on_actionToggleCheckerboard_triggered();
    void on_actionToggleStayOnTop_triggered();
    void on_actionToggleProtectMode_triggered();
    void on_actionSettings_triggered();
    void on_actionHelp_triggered();
    void on_actionProperties_triggered();
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
