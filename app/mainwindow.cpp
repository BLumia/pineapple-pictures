// SPDX-FileCopyrightText: 2022 Gary Wang <wzc782970009@gmail.com>
//
// SPDX-License-Identifier: MIT

#include "mainwindow.h"

#include "settings.h"
#include "toolbutton.h"
#include "bottombuttongroup.h"
#include "graphicsview.h"
#include "navigatorview.h"
#include "graphicsscene.h"
#include "settingsdialog.h"
#include "aboutdialog.h"
#include "metadatamodel.h"
#include "metadatadialog.h"
#include "actionmanager.h"
#include "playlistmanager.h"

#include <QMouseEvent>
#include <QMovie>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QApplication>
#include <QStyle>
#include <QScreen>
#include <QMenu>
#include <QShortcut>
#include <QClipboard>
#include <QMimeData>
#include <QWindow>
#include <QFile>
#include <QTimer>
#include <QFileDialog>
#include <QStandardPaths>
#include <QProcess>
#include <QDesktopServices>

MainWindow::MainWindow(QWidget *parent)
    : FramelessWindow(parent)
    , m_am(new ActionManager)
    , m_pm(new PlaylistManager(PlaylistManager::PL_SAMEFOLDER, this))
{
    if (Settings::instance()->stayOnTop()) {
        this->setWindowFlag(Qt::WindowStaysOnTopHint);
    }

    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setMinimumSize(350, 330);
    this->setWindowIcon(QIcon(":/icons/app-icon.svg"));
    this->setMouseTracking(true);

    m_pm->setAutoLoadFilterSuffix({"*.jpg", "*.jpeg", "*.jfif", "*.png", "*.gif", "*.svg", "*.bmp", "*.webp"});

    m_fadeOutAnimation = new QPropertyAnimation(this, "windowOpacity");
    m_fadeOutAnimation->setDuration(300);
    m_fadeOutAnimation->setStartValue(1);
    m_fadeOutAnimation->setEndValue(0);
    m_floatUpAnimation = new QPropertyAnimation(this, "geometry");
    m_floatUpAnimation->setDuration(300);
    m_floatUpAnimation->setEasingCurve(QEasingCurve::OutCirc);
    m_exitAnimationGroup = new QParallelAnimationGroup(this);
    m_exitAnimationGroup->addAnimation(m_fadeOutAnimation);
    m_exitAnimationGroup->addAnimation(m_floatUpAnimation);
    connect(m_exitAnimationGroup, &QParallelAnimationGroup::finished,
            this, &QWidget::close);

    GraphicsScene * scene = new GraphicsScene(this);

    m_graphicsView = new GraphicsView(this);
    m_graphicsView->setScene(scene);
    this->setCentralWidget(m_graphicsView);

    m_gv = new NavigatorView(this);
    m_gv->setFixedSize(220, 160);
    m_gv->setScene(scene);
    m_gv->setMainView(m_graphicsView);
    m_gv->fitInView(m_gv->sceneRect(), Qt::KeepAspectRatio);

    connect(m_graphicsView, &GraphicsView::navigatorViewRequired,
            this, [ = ](bool required, const QTransform & tf){
        m_gv->setTransform(GraphicsView::resetScale(tf));
        m_gv->fitInView(m_gv->sceneRect(), Qt::KeepAspectRatio);
        m_gv->setVisible(required);
        m_gv->updateMainViewportRegion();
    });

    connect(m_graphicsView, &GraphicsView::viewportRectChanged,
            m_gv, &NavigatorView::updateMainViewportRegion);

    connect(m_graphicsView, &GraphicsView::requestGallery,
            this, &MainWindow::loadGalleryBySingleLocalFile);

    m_closeButton = new ToolButton(true, m_graphicsView);
    m_closeButton->setIconSize(QSize(32, 32));
    m_closeButton->setFixedSize(QSize(50, 50));
    m_closeButton->setIconResourcePath(":/icons/window-close.svg");

    connect(m_closeButton, &QAbstractButton::clicked,
            this, &MainWindow::closeWindow);

    m_prevButton = new ToolButton(false, m_graphicsView);
    m_prevButton->setIconSize(QSize(75, 75));
    m_prevButton->setIconResourcePath(":/icons/go-previous.svg");
    m_prevButton->setVisible(false);
    m_prevButton->setOpacity(0, false);
    m_nextButton = new ToolButton(false, m_graphicsView);
    m_nextButton->setIconSize(QSize(75, 75));
    m_nextButton->setIconResourcePath(":/icons/go-next.svg");
    m_nextButton->setVisible(false);
    m_nextButton->setOpacity(0, false);

    connect(m_prevButton, &QAbstractButton::clicked,
            this, &MainWindow::galleryPrev);
    connect(m_nextButton, &QAbstractButton::clicked,
            this, &MainWindow::galleryNext);

    m_am->setupAction(this);

    m_bottomButtonGroup = new BottomButtonGroup({
        m_am->actionActualSize,
        m_am->actionToggleMaximize,
        m_am->actionZoomIn,
        m_am->actionZoomOut,
        m_am->actionToggleCheckerboard,
        m_am->actionRotateClockwise
    }, this);

    m_bottomButtonGroup->setOpacity(0, false);
    m_gv->setOpacity(0, false);
    m_closeButton->setOpacity(0, false);

    connect(m_pm, &PlaylistManager::loaded, this, [this](int galleryFileCount) {
        m_prevButton->setVisible(galleryFileCount > 1);
        m_nextButton->setVisible(galleryFileCount > 1);
    });

    QShortcut * fullscreenShorucut = new QShortcut(QKeySequence(QKeySequence::FullScreen), this);
    connect(fullscreenShorucut, &QShortcut::activated,
            this, &MainWindow::toggleFullscreen);

    centerWindow();

    QTimer::singleShot(0, this, [this](){
        m_am->setupShortcuts();
    });
}

MainWindow::~MainWindow()
{

}

void MainWindow::showUrls(const QList<QUrl> &urls)
{
    if (!urls.isEmpty()) {
        if (urls.count() == 1) {
            m_graphicsView->showFileFromPath(urls.first().toLocalFile(), true);
        } else {
            m_graphicsView->showFileFromPath(urls.first().toLocalFile(), false);
            m_pm->setPlaylist(urls);
            m_pm->setCurrentIndex(0);
        }
    } else {
        m_graphicsView->showText(tr("File url list is empty"));
        return;
    }

    m_gv->fitInView(m_gv->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::initWindowSize()
{
    switch (Settings::instance()->initWindowSizeBehavior()) {
    case Settings::WindowSizeBehavior::Auto:
        adjustWindowSizeBySceneRect();
        break;
    case Settings::WindowSizeBehavior::Maximized:
        showMaximized();
        break;
    default:
        adjustWindowSizeBySceneRect();
        break;
    }
}

void MainWindow::adjustWindowSizeBySceneRect()
{
    if (m_pm->count() < 1) return;

    QSize sceneSize = m_graphicsView->sceneRect().toRect().size();
    QSize sceneSizeWithMargins = sceneSize + QSize(130, 125);

    if (m_graphicsView->scaleFactor() < 1 || size().expandedTo(sceneSizeWithMargins) != size()) {
        // if it scaled down by the resize policy:
        QSize screenSize = qApp->screenAt(QCursor::pos())->availableSize();
        if (screenSize.expandedTo(sceneSize) == screenSize) {
            // we can show the picture by increase the window size.
            QSize finalSize = (screenSize.expandedTo(sceneSizeWithMargins) == screenSize) ?
                              sceneSizeWithMargins : screenSize;
            // We have a very reasonable sizeHint() value ;P
            this->resize(finalSize.expandedTo(this->sizeHint()));

            // We're sure the window can display the whole thing with 1:1 scale.
            // The old window size may cause fitInView call from resize() and the
            // above resize() call won't reset the scale back to 1:1, so we
            // just call resetScale() here to ensure the thing is no longer scaled.
            m_graphicsView->resetScale();
            centerWindow();
        } else {
            // toggle maximum
            showMaximized();
        }
    }
}

// can be empty if it is NOT from a local file.
QUrl MainWindow::currentImageFileUrl() const
{
    QUrl url;
    std::tie(std::ignore, url) = m_pm->currentFileUrl();

    return url;
}

void MainWindow::clearGallery()
{
    m_pm->clear();
}

void MainWindow::loadGalleryBySingleLocalFile(const QString &path)
{
    m_pm->setCurrentFile(path);
}

void MainWindow::galleryPrev()
{
    int index;
    QString filePath;
    std::tie(index, filePath) = m_pm->previousFile();

    if (index >= 0) {
        m_graphicsView->showFileFromPath(filePath, false);
        m_pm->setCurrentIndex(index);
    }
}

void MainWindow::galleryNext()
{
    int index;
    QString filePath;
    std::tie(index, filePath) = m_pm->nextFile();

    if (index >= 0) {
        m_graphicsView->showFileFromPath(filePath, false);
        m_pm->setCurrentIndex(index);
    }
}

void MainWindow::showEvent(QShowEvent *event)
{
    updateWidgetsPosition();

    return FramelessWindow::showEvent(event);
}

void MainWindow::enterEvent(QT_ENTER_EVENT *event)
{
    m_bottomButtonGroup->setOpacity(1);
    m_gv->setOpacity(1);

    m_closeButton->setOpacity(1);
    m_prevButton->setOpacity(1);
    m_nextButton->setOpacity(1);

    return FramelessWindow::enterEvent(event);
}

void MainWindow::leaveEvent(QEvent *event)
{
    m_bottomButtonGroup->setOpacity(0);
    m_gv->setOpacity(0);

    m_closeButton->setOpacity(0);
    m_prevButton->setOpacity(0);
    m_nextButton->setOpacity(0);

    return FramelessWindow::leaveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && !isMaximized()) {
        m_clickedOnWindow = true;
        m_oldMousePos = event->pos();
//        qDebug() << m_oldMousePos << m_graphicsView->transform().m11()
//                 << m_graphicsView->transform().m22() << m_graphicsView->matrix().m12();
        event->accept();
    }

    return FramelessWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_clickedOnWindow && !isMaximized()) {
#if QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        if (!window()->windowHandle()->startSystemMove()) {
            move(event->globalPos() - m_oldMousePos);
        }
#else
        move(event->globalPos() - m_oldMousePos);
#endif // QT_VERSION >= QT_VERSION_CHECK(5, 15, 0)
        event->accept();
    }

    return FramelessWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_clickedOnWindow = false;

    // It seems the forward/back mouse button won't generate a key event [1] so we can't use
    // QShortcut or QKeySequence to indicate these shortcuts, so we do it here.
    // Reference:
    // [1]: https://codereview.qt-project.org/c/qt/qtbase/+/177475
    if (event->button() == Qt::ForwardButton || event->button() == Qt::BackButton) {
        event->button() == Qt::BackButton ? galleryPrev() : galleryNext();
        event->accept();
    }

    return FramelessWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    // The forward/back mouse button can also used to trigger a mouse double-click event
    // Since we use that for gallery navigation so we ignore these two buttons.
    if (event->buttons() & Qt::ForwardButton || event->buttons() & Qt::BackButton) {
        return;
    }

    switch (Settings::instance()->doubleClickBehavior()) {
    case Settings::DoubleClickBehavior::Close:
        quitAppAction();
        event->accept();
        break;
    case Settings::DoubleClickBehavior::Maximize:
        toggleMaximize();
        event->accept();
        break;
    case Settings::DoubleClickBehavior::Ignore:
        break;
    }

    // blumia: don't call parent constructor here, seems it will cause mouse move
    //         event get called even if we set event->accept();
    // return QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    QPoint numDegrees = event->angleDelta() / 8;
    bool needWeelEvent = false, wheelUp = false;
    bool actionIsZoom = event->modifiers().testFlag(Qt::ControlModifier)
            || Settings::instance()->mouseWheelBehavior() == Settings::MouseWheelBehavior::Zoom;

    // NOTE: Only checking angleDelta since the QWheelEvent::pixelDelta() doc says
    //       pixelDelta() value is driver specific and unreliable on X11...
    //       We are not scrolling the canvas, just zoom in or out, so it probably
    //       doesn't matter here.
    if (!numDegrees.isNull() && numDegrees.y() != 0) {
        needWeelEvent = true;
        wheelUp = numDegrees.y() > 0;
    }

    if (needWeelEvent) {
        if (actionIsZoom) {
            if (wheelUp) {
                on_actionZoomIn_triggered();
            } else {
                on_actionZoomOut_triggered();
            }
        } else {
            if (wheelUp) {
                galleryPrev();
            } else {
                galleryNext();
            }
        }
        event->accept();
    } else {
        FramelessWindow::wheelEvent(event);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    updateWidgetsPosition();

    return FramelessWindow::resizeEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu * menu = new QMenu;
    QMenu * copyMenu = new QMenu(tr("&Copy"));
    QUrl currentFileUrl = currentImageFileUrl();
    QImage clipboardImage;
    QUrl clipboardFileUrl;

    QAction * copyPixmap = m_am->actionCopyPixmap;
    QAction * copyFilePath = m_am->actionCopyFilePath;

    copyMenu->addAction(copyPixmap);
    if (currentFileUrl.isValid()) {
        copyMenu->addAction(copyFilePath);
    }

    QAction * paste = m_am->actionPaste;

    QAction * stayOnTopMode = m_am->actionToggleStayOnTop;
    stayOnTopMode->setCheckable(true);
    stayOnTopMode->setChecked(stayOnTop());

    QAction * protectedMode = m_am->actionToggleProtectMode;
    protectedMode->setCheckable(true);
    protectedMode->setChecked(m_protectedMode);

    QAction * toggleSettings = m_am->actionSettings;
    QAction * helpAction = m_am->actionHelp;
    QAction * propertiesAction = m_am->actionProperties;

#if 0
    menu->addAction(m_am->actionOpen);
#endif // 0

    if (copyMenu->actions().count() == 1) {
        menu->addActions(copyMenu->actions());
    } else {
        menu->addMenu(copyMenu);
    }

    if (canPaste()) {
        menu->addAction(paste);
    }

    menu->addSeparator();

    menu->addAction(m_am->actionHorizontalFlip);
#if 0
    menu->addAction(m_am->actionFitInView);
    menu->addAction(m_am->actionFitByWidth);
#endif // 0
    menu->addSeparator();
    menu->addAction(stayOnTopMode);
    menu->addAction(protectedMode);
    menu->addSeparator();
    menu->addAction(toggleSettings);
    menu->addAction(helpAction);
    if (currentFileUrl.isValid()) {
        menu->addSeparator();
        if (currentFileUrl.isLocalFile()) {
            menu->addAction(m_am->actionLocateInFileManager);
        }
        menu->addAction(propertiesAction);
    }
    menu->exec(mapToGlobal(event->pos()));
    menu->deleteLater();
    copyMenu->deleteLater();

    return FramelessWindow::contextMenuEvent(event);
}

void MainWindow::centerWindow()
{
    this->setGeometry(
        QStyle::alignedRect(
            Qt::LeftToRight,
            Qt::AlignCenter,
            this->size(),
            qApp->screenAt(QCursor::pos())->geometry()
        )
    );
}

void MainWindow::closeWindow()
{
    QRect windowRect(this->geometry());
    m_floatUpAnimation->setStartValue(windowRect);
    m_floatUpAnimation->setEndValue(windowRect.adjusted(0, -80, 0, 0));
    m_floatUpAnimation->setStartValue(QRect(this->geometry().x(), this->geometry().y(), this->geometry().width(), this->geometry().height()));
    m_floatUpAnimation->setEndValue(QRect(this->geometry().x(), this->geometry().y()-80, this->geometry().width(), this->geometry().height()));
    m_exitAnimationGroup->start();
}

void MainWindow::updateWidgetsPosition()
{
    m_closeButton->move(width() - m_closeButton->width(), 0);
    m_prevButton->move(25, (height() - m_prevButton->sizeHint().height()) / 2);
    m_nextButton->move(width() - m_nextButton->sizeHint().width() - 25,
                       (height() - m_prevButton->sizeHint().height()) / 2);
    m_bottomButtonGroup->move((width() - m_bottomButtonGroup->width()) / 2,
                              height() - m_bottomButtonGroup->height());
    m_gv->move(width() - m_gv->width(), height() - m_gv->height());
}

void MainWindow::toggleProtectedMode()
{
    m_protectedMode = !m_protectedMode;
    m_closeButton->setVisible(!m_protectedMode);
    m_prevButton->setVisible(!m_protectedMode);
    m_nextButton->setVisible(!m_protectedMode);
}

void MainWindow::toggleStayOnTop()
{
    setWindowFlag(Qt::WindowStaysOnTopHint, !stayOnTop());
    show();
}

bool MainWindow::stayOnTop() const
{
    return windowFlags().testFlag(Qt::WindowStaysOnTopHint);
}

bool MainWindow::canPaste() const
{
    const QMimeData * clipboardData = QApplication::clipboard()->mimeData();
    if (clipboardData->hasImage()) {
        return true;
    } else if (clipboardData->hasText()) {
        QString clipboardText(clipboardData->text());
        if (clipboardText.startsWith("PICTURE:")) {
            QString maybeFilename(clipboardText.mid(8));
            if (QFile::exists(maybeFilename)) {
                return true;
            }
        }
    }

    return false;
}

void MainWindow::quitAppAction(bool force)
{
    if (!m_protectedMode || force) {
        closeWindow();
    }
}

void MainWindow::toggleFullscreen()
{
    if (isFullScreen()) {
        showNormal();
    } else {
        showFullScreen();
    }
}

void MainWindow::toggleMaximize()
{
    if (isMaximized()) {
        showNormal();
    } else {
        showMaximized();
    }
}

QSize MainWindow::sizeHint() const
{
    return QSize(710, 530);
}

void MainWindow::on_actionOpen_triggered()
{
    QStringList picturesLocations = QStandardPaths::standardLocations(QStandardPaths::PicturesLocation);
    QUrl pictureUrl = picturesLocations.isEmpty() ? QUrl::fromLocalFile(picturesLocations.first())
                                                  : QUrl::fromLocalFile(QDir::homePath());
    QList<QUrl> urls(QFileDialog::getOpenFileUrls(this, QString(), pictureUrl));
    if (!urls.isEmpty()) {
        showUrls(urls);
    }
}

void MainWindow::on_actionActualSize_triggered()
{
    m_graphicsView->resetScale();
    m_graphicsView->setEnableAutoFitInView(false);
}

void MainWindow::on_actionToggleMaximize_triggered()
{
    toggleMaximize();
}

void MainWindow::on_actionZoomIn_triggered()
{
    if (m_graphicsView->scaleFactor() < 1000) {
        m_graphicsView->zoomView(1.25);
    }
}

void MainWindow::on_actionZoomOut_triggered()
{
    m_graphicsView->zoomView(0.8);
}

void MainWindow::on_actionHorizontalFlip_triggered()
{
    m_graphicsView->flipView();
}

void MainWindow::on_actionFitInView_triggered()
{
    m_graphicsView->fitInView(m_gv->sceneRect(), Qt::KeepAspectRatio);
    m_graphicsView->setEnableAutoFitInView(m_graphicsView->scaleFactor() <= 1);
}

void MainWindow::on_actionFitByWidth_triggered()
{
    m_graphicsView->fitByOrientation();
}

void MainWindow::on_actionCopyPixmap_triggered()
{
    QClipboard *cb = QApplication::clipboard();
    cb->setPixmap(m_graphicsView->scene()->renderToPixmap());
}

void MainWindow::on_actionCopyFilePath_triggered()
{
    QUrl currentFileUrl(currentImageFileUrl());
    if (currentFileUrl.isValid()) {
        QClipboard *cb = QApplication::clipboard();
        cb->setText(currentFileUrl.toLocalFile());
    }
}

void MainWindow::on_actionPaste_triggered()
{
    QImage clipboardImage;
    QUrl clipboardFileUrl;

    const QMimeData * clipboardData = QApplication::clipboard()->mimeData();
    if (clipboardData->hasImage()) {
        QVariant imageVariant(clipboardData->imageData());
        if (imageVariant.isValid()) {
            clipboardImage = qvariant_cast<QImage>(imageVariant);
        }
    } else if (clipboardData->hasText()) {
        QString clipboardText(clipboardData->text());
        if (clipboardText.startsWith("PICTURE:")) {
            QString maybeFilename(clipboardText.mid(8));
            if (QFile::exists(maybeFilename)) {
                clipboardFileUrl = QUrl::fromLocalFile(maybeFilename);
            }
        }
    }

    if (!clipboardImage.isNull()) {
        m_graphicsView->showImage(clipboardImage);
        clearGallery();
    } else if (clipboardFileUrl.isValid()) {
        QString localFile(clipboardFileUrl.toLocalFile());
        m_graphicsView->showFileFromPath(localFile, true);
        m_pm->setCurrentFile(localFile);
    }
}

void MainWindow::on_actionToggleCheckerboard_triggered()
{
    // TODO: is that okay to do this since we plan to support custom shortcuts?
    m_graphicsView->toggleCheckerboard(QGuiApplication::queryKeyboardModifiers().testFlag(Qt::ShiftModifier));
}

void MainWindow::on_actionRotateClockwise_triggered()
{
    m_graphicsView->rotateView();
    m_graphicsView->displayScene();
    m_gv->setVisible(false);
}

void MainWindow::on_actionPrevPicture_triggered()
{
    galleryPrev();
}

void MainWindow::on_actionNextPicture_triggered()
{
    galleryNext();
}

void MainWindow::on_actionToggleStayOnTop_triggered()
{
    toggleStayOnTop();
}

void MainWindow::on_actionToggleProtectMode_triggered()
{
    toggleProtectedMode();
}

void MainWindow::on_actionSettings_triggered()
{
    SettingsDialog * sd = new SettingsDialog(this);
    sd->exec();
    sd->deleteLater();
}

void MainWindow::on_actionHelp_triggered()
{
    AboutDialog * ad = new AboutDialog(this);
    ad->exec();
    ad->deleteLater();
}

void MainWindow::on_actionProperties_triggered()
{
    QUrl currentFileUrl = currentImageFileUrl();
    if (!currentFileUrl.isValid()) return;

    MetadataModel * md = new MetadataModel();
    md->setFile(currentFileUrl.toLocalFile());

    MetadataDialog * ad = new MetadataDialog(this);
    ad->setMetadataModel(md);
    ad->exec();
    ad->deleteLater();
}

void MainWindow::on_actionLocateInFileManager_triggered()
{
    QUrl currentFileUrl = currentImageFileUrl();
    if (!currentFileUrl.isValid()) return;

    QFileInfo fileInfo(currentFileUrl.toLocalFile());
    if (!fileInfo.exists()) return;

#ifdef Q_OS_WIN
    QProcess::startDetached("explorer", QStringList() << "/select," << QDir::toNativeSeparators(fileInfo.absoluteFilePath()));
#else
    // maybe use https://www.freedesktop.org/wiki/Specifications/file-manager-interface/ for linux?
    QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
#endif // Q_OS_WIN
}

void MainWindow::on_actionQuitApp_triggered()
{
    quitAppAction(false);
}
