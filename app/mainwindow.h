#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QParallelAnimationGroup>
#include <QPropertyAnimation>
#include <QPushButton>

QT_BEGIN_NAMESPACE
class QGraphicsOpacityEffect;
class QGraphicsView;
QT_END_NAMESPACE

class ToolButton;
class GraphicsView;
class NavigatorView;
class BottomButtonGroup;
class MainWindow : public QMainWindow
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
    bool isGalleryAvailable();

signals:
    void galleryLoaded();

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

    bool nativeEvent(const QByteArray& eventType, void* message, long* result) override;

    QSize sizeHint() const override;

    void centerWindow();
    void closeWindow();
    void updateWidgetsPosition();
    void toggleProtectedMode();
    void toggleStayOnTop();
    bool stayOnTop();
    void quitAppAction(bool force = false);
    void toggleFullscreen();
    void toggleMaximize();

private:
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

    QList<QUrl> m_files;
    int m_currentFileIndex = -1;
};

#endif // MAINWINDOW_H
