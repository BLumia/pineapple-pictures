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

#include <QMouseEvent>
#include <QMovie>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QApplication>
#include <QStyle>
#include <QScreen>
#include <QMenu>
#include <QShortcut>
#include <QDir>
#include <QCollator>
#include <QClipboard>
#include <QMimeData>
#include <QWindow>
#include <QTimer>

MainWindow::MainWindow(QWidget *parent)
    : FramelessWindow(parent)
    , m_am(new ActionManager)
{
    if (Settings::instance()->stayOnTop()) {
        this->setWindowFlag(Qt::WindowStaysOnTopHint);
    }

    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setMinimumSize(350, 330);
    this->setWindowIcon(QIcon(":/icons/app-icon.svg"));
    this->setMouseTracking(true);

    m_fadeOutAnimation = new QPropertyAnimation(this, "windowOpacity");
    m_fadeOutAnimation->setDuration(300);
    m_fadeOutAnimation->setStartValue(1);
    m_fadeOutAnimation->setEndValue(0);
    m_floatUpAnimation = new QPropertyAnimation(this, "geometry");
    m_floatUpAnimation->setDuration(300);
    m_floatUpAnimation->setEasingCurve(QEasingCurve::OutCirc);
    m_exitAnimationGroup = new QParallelAnimationGroup;
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
            this, [ = ](bool required, qreal angle){
        m_gv->resetTransform();
        m_gv->rotate(angle);
        m_gv->fitInView(m_gv->sceneRect(), Qt::KeepAspectRatio);
        m_gv->setVisible(required);
        m_gv->updateMainViewportRegion();
    });

    connect(m_graphicsView, &GraphicsView::viewportRectChanged,
            m_gv, &NavigatorView::updateMainViewportRegion);

    connect(m_graphicsView, &GraphicsView::requestGallery,
            this, &MainWindow::loadGalleryBySingleLocalFile);

    m_closeButton = new ToolButton(true, m_graphicsView);
    m_closeButton->setIcon(QIcon(":/icons/window-close"));
    m_closeButton->setIconSize(QSize(50, 50));

    connect(m_closeButton, &QAbstractButton::clicked,
            this, &MainWindow::closeWindow);

    m_prevButton = new ToolButton(false, m_graphicsView);
    m_prevButton->setIcon(QIcon(":/icons/go-previous"));
    m_prevButton->setIconSize(QSize(75, 75));
    m_prevButton->setVisible(false);
    m_prevButton->setOpacity(0, false);
    m_nextButton = new ToolButton(false, m_graphicsView);
    m_nextButton->setIcon(QIcon(":/icons/go-next"));
    m_nextButton->setIconSize(QSize(75, 75));
    m_nextButton->setVisible(false);
    m_nextButton->setOpacity(0, false);

    connect(m_prevButton, &QAbstractButton::clicked,
            this, &MainWindow::galleryPrev);
    connect(m_nextButton, &QAbstractButton::clicked,
            this, &MainWindow::galleryNext);

    m_bottomButtonGroup = new BottomButtonGroup(this);

    connect(m_bottomButtonGroup, &BottomButtonGroup::resetToOriginalBtnClicked,
            this, [ = ](){ m_graphicsView->resetScale(); });
    connect(m_bottomButtonGroup, &BottomButtonGroup::toggleWindowMaximum,
            this, &MainWindow::toggleMaximize);
    connect(m_bottomButtonGroup, &BottomButtonGroup::zoomInBtnClicked,
            this, [ = ](){ m_graphicsView->zoomView(1.25); });
    connect(m_bottomButtonGroup, &BottomButtonGroup::zoomOutBtnClicked,
            this, [ = ](){ m_graphicsView->zoomView(0.8); });
    connect(m_bottomButtonGroup, &BottomButtonGroup::toggleCheckerboardBtnClicked,
            this, [ = ](){ m_graphicsView->toggleCheckerboard(); });
    connect(m_bottomButtonGroup, &BottomButtonGroup::rotateRightBtnClicked,
            this, [ = ](){
        m_graphicsView->resetScale();
        m_graphicsView->rotateView(90);
        m_graphicsView->checkAndDoFitInView();
        m_gv->setVisible(false);
    });

    m_bottomButtonGroup->setOpacity(0, false);
    m_gv->setOpacity(0, false);
    m_closeButton->setOpacity(0, false);

    connect(this, &MainWindow::galleryLoaded, this, [this]() {
        m_prevButton->setVisible(isGalleryAvailable());
        m_nextButton->setVisible(isGalleryAvailable());
    });

    QShortcut * prevPictureShorucut = new QShortcut(QKeySequence(Qt::Key_PageUp), this);
    connect(prevPictureShorucut, &QShortcut::activated,
            this, &MainWindow::galleryPrev);

    QShortcut * nextPictureShorucut = new QShortcut(QKeySequence(Qt::Key_PageDown), this);
    connect(nextPictureShorucut, &QShortcut::activated,
            this, &MainWindow::galleryNext);

    QShortcut * fullscreenShorucut = new QShortcut(QKeySequence(QKeySequence::FullScreen), this);
    connect(fullscreenShorucut, &QShortcut::activated,
            this, &MainWindow::toggleFullscreen);

    m_am->setupAction(this);

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
            m_graphicsView->showFileFromUrl(urls.first(), true);
        } else {
            m_graphicsView->showFileFromUrl(urls.first(), false);
            m_files = urls;
            m_currentFileIndex = 0;
        }
    } else {
        m_graphicsView->showText(tr("File url list is empty"));
        return;
    }

    m_gv->fitInView(m_gv->sceneRect(), Qt::KeepAspectRatio);
}

void MainWindow::adjustWindowSizeBySceneRect()
{
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
    if (m_currentFileIndex != -1) {
        return m_files.value(m_currentFileIndex);
    }

    return QUrl();
}

void MainWindow::clearGallery()
{
    m_currentFileIndex = -1;
    m_files.clear();
}

void MainWindow::loadGalleryBySingleLocalFile(const QString &path)
{
    QFileInfo info(path);
    QDir dir(info.path());
    QString currentFileName = info.fileName();
    QStringList entryList = dir.entryList({"*.jpg", "*.jpeg", "*.jfif", "*.png", "*.gif", "*.svg", "*.bmp"},
                                          QDir::Files | QDir::NoSymLinks, QDir::NoSort);

    QCollator collator;
    collator.setNumericMode(true);

    std::sort(entryList.begin(), entryList.end(), collator);

    clearGallery();

    for (int i = 0; i < entryList.count(); i++) {
        const QString & fileName = entryList.at(i);
        const QString & oneEntry = dir.absoluteFilePath(fileName);
        const QUrl & url = QUrl::fromLocalFile(oneEntry);
        m_files.append(url);
        if (fileName == currentFileName) {
            m_currentFileIndex = i;
        }
    }

    emit galleryLoaded();
}

void MainWindow::galleryPrev()
{
    int count = m_files.count();
    if (!isGalleryAvailable()) {
        return;
    }

    m_currentFileIndex = m_currentFileIndex - 1 < 0 ? count - 1 : m_currentFileIndex - 1;

    m_graphicsView->showFileFromUrl(m_files.at(m_currentFileIndex), false);
}

void MainWindow::galleryNext()
{
    int count = m_files.count();
    if (!isGalleryAvailable()) {
        return;
    }

    m_currentFileIndex = m_currentFileIndex + 1 == count ? 0 : m_currentFileIndex + 1;

    m_graphicsView->showFileFromUrl(m_files.at(m_currentFileIndex), false);
}

bool MainWindow::isGalleryAvailable()
{
    if (m_currentFileIndex < 0 || m_files.isEmpty() || m_currentFileIndex >= m_files.count()) {
        return false;
    }
    return true;
}

void MainWindow::showEvent(QShowEvent *event)
{
    updateWidgetsPosition();

    return FramelessWindow::showEvent(event);
}

void MainWindow::enterEvent(QEvent *event)
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

    return FramelessWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    switch (Settings::instance()->doubleClickBehavior()) {
    case ActionCloseWindow:
        quitAppAction();
        event->accept();
        break;
    case ActionMaximizeWindow:
        toggleMaximize();
        event->accept();
        break;
    case ActionDoNothing:
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
            || Settings::instance()->mouseWheelBehavior() == ActionZoomImage;

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
            m_graphicsView->zoomView(wheelUp ? 1.25 : 0.8);
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

    if (copyMenu->actions().count() == 1) {
        menu->addActions(copyMenu->actions());
    } else {
        menu->addMenu(copyMenu);
    }

    if (canPaste()) {
        menu->addAction(paste);
    }

    menu->addSeparator();
    menu->addAction(stayOnTopMode);
    menu->addAction(protectedMode);
    menu->addSeparator();
    menu->addAction(toggleSettings);
    menu->addAction(helpAction);
    if (currentFileUrl.isValid()) {
        menu->addSeparator();
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
    m_prevButton->move(25, (height() - m_prevButton->height()) / 2);
    m_nextButton->move(width() - m_nextButton->width() - 25,
                       (height() - m_prevButton->height()) / 2);
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

bool MainWindow::stayOnTop()
{
    return windowFlags().testFlag(Qt::WindowStaysOnTopHint);
}

bool MainWindow::canPaste()
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
        clearGallery();
        m_graphicsView->showImage(clipboardImage);
    } else if (clipboardFileUrl.isValid()) {
        m_graphicsView->showFileFromUrl(clipboardFileUrl, true);
    }
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

void MainWindow::on_actionQuitApp_triggered()
{
    quitAppAction(false);
}
