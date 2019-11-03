#include "mainwindow.h"

#include "toolbutton.h"
#include "bottombuttongroup.h"
#include "graphicsview.h"
#include "navigatorview.h"
#include "graphicsscene.h"

#include <QMouseEvent>
#include <QMovie>
#include <QDebug>
#include <QGraphicsTextItem>
#include <QApplication>
#include <QStyle>
#include <QScreen>
#include <QMenu>
#include <QShortcut>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent)
{
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    this->setAttribute(Qt::WA_TranslucentBackground, true);
    this->setMinimumSize(710, 530);
    this->setWindowIcon(QIcon(":/icons/app-icon.svg"));

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
            this, &QMainWindow::close);

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

    m_closeButton = new ToolButton(m_graphicsView);
    m_closeButton->setIcon(QIcon(":/icons/window-close"));
    m_closeButton->setIconSize(QSize(50, 50));

    connect(m_closeButton, &QAbstractButton::clicked,
            this, &MainWindow::closeWindow);

    m_bottomButtonGroup = new BottomButtonGroup(this);

    connect(m_bottomButtonGroup, &BottomButtonGroup::resetToOriginalBtnClicked,
            this, [ = ](){ m_graphicsView->resetScale(); });
    connect(m_bottomButtonGroup, &BottomButtonGroup::toggleWindowMaximum,
            this, [ = ](){
        if (isMaximized()) {
            showNormal();
        } else {
            showMaximized();
        }
    });
    connect(m_bottomButtonGroup, &BottomButtonGroup::zoomInBtnClicked,
            this, [ = ](){ m_graphicsView->zoomView(1.25); });
    connect(m_bottomButtonGroup, &BottomButtonGroup::zoomOutBtnClicked,
            this, [ = ](){ m_graphicsView->zoomView(0.75); });
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

    QShortcut * quitAppShorucut = new QShortcut(QKeySequence(Qt::Key_Space), this);
    connect(quitAppShorucut, &QShortcut::activated,
            std::bind(&MainWindow::quitAppAction, this, false));

    centerWindow();
}

MainWindow::~MainWindow()
{

}

void MainWindow::showUrls(const QList<QUrl> &urls)
{
    m_graphicsView->showFromUrlList(urls);
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
            if (screenSize.expandedTo(sceneSizeWithMargins) == screenSize) {
                this->resize(sceneSizeWithMargins);
            } else {
                this->resize(screenSize);
            }
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

void MainWindow::showEvent(QShowEvent *event)
{
    updateWidgetsPosition();

    return QMainWindow::showEvent(event);
}

void MainWindow::enterEvent(QEvent *event)
{
    m_bottomButtonGroup->setOpacity(1);
    m_gv->setOpacity(1);

    m_closeButton->setOpacity(1);

    return QMainWindow::enterEvent(event);
}

void MainWindow::leaveEvent(QEvent *event)
{
    m_bottomButtonGroup->setOpacity(0);
    m_gv->setOpacity(0);

    m_closeButton->setOpacity(0);

    return QMainWindow::leaveEvent(event);
}

void MainWindow::mousePressEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && !isMaximized()) {
        m_clickedOnWindow = true;
        m_oldMousePos = event->pos();
        qDebug() << m_oldMousePos << m_graphicsView->transform().m11()
                 << m_graphicsView->transform().m22() << m_graphicsView->matrix().m12();
        event->accept();
    }

    return QMainWindow::mousePressEvent(event);
}

void MainWindow::mouseMoveEvent(QMouseEvent *event)
{
    if (event->buttons() & Qt::LeftButton && m_clickedOnWindow) {
        move(event->globalPos() - m_oldMousePos);
        event->accept();
    }

    return QMainWindow::mouseMoveEvent(event);
}

void MainWindow::mouseReleaseEvent(QMouseEvent *event)
{
    m_clickedOnWindow = false;

    return QMainWindow::mouseReleaseEvent(event);
}

void MainWindow::mouseDoubleClickEvent(QMouseEvent *event)
{
    quitAppAction();

    return QMainWindow::mouseDoubleClickEvent(event);
}

void MainWindow::wheelEvent(QWheelEvent *event)
{
    if (event->delta() > 0) {
        m_graphicsView->zoomView(1.25);
    } else {
        m_graphicsView->zoomView(0.8);
    }
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
    updateWidgetsPosition();

    return QMainWindow::resizeEvent(event);
}

void MainWindow::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu * menu = new QMenu;
    QAction * stayOnTopMode = new QAction(tr("Stay on top"));
    connect(stayOnTopMode, &QAction::triggered, this, [ = ](){
        toggleStayOnTop();
    });
    stayOnTopMode->setCheckable(true);
    stayOnTopMode->setChecked(stayOnTop());
    QAction * protectedMode = new QAction(tr("Protected mode"));
    connect(protectedMode, &QAction::triggered, this, [ = ](){
        toggleProtectedMode();
    });
    protectedMode->setCheckable(true);
    protectedMode->setChecked(m_protectedMode);
    QAction * helpAction = new QAction(tr("Help"));
    connect(helpAction, &QAction::triggered, this, [ = ](){
        QStringList sl {
            tr("Launch application with image file path as argument to load the file."),
            tr("Drag and drop image file onto the window is also supported."),
            "",
            tr("Context menu option explanation:"),
            (tr("Stay on top") + " : " + tr("Make window stay on top of all other windows.")),
            (tr("Protected mode") + " : " + tr("Avoid close window accidentally. (eg. by double clicking the window)"))
        };
        m_graphicsView->showText(sl.join('\n'));
    });
    menu->addAction(stayOnTopMode);
    menu->addAction(protectedMode);
    menu->addSeparator();
    menu->addAction(helpAction);
    menu->exec(mapToGlobal(event->pos()));
    menu->deleteLater();

    return QMainWindow::contextMenuEvent(event);
}

bool MainWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
#ifdef _WIN32
    // https://stackoverflow.com/questions/43505580/qt-windows-resizable-frameless-window
    // Too lazy to do this now.. just stackoverflow it and did a copy and paste..
    Q_UNUSED(eventType)
    MSG* msg = static_cast<MSG*>(message);

    if (msg->message == WM_NCHITTEST) {
        if (isMaximized()) {
            return false;
        }

        *result = 0;
        const LONG borderWidth = 8;
        RECT winrect;
        GetWindowRect(reinterpret_cast<HWND>(winId()), &winrect);

        // must be short to correctly work with multiple monitors (negative coordinates)
        short x = msg->lParam & 0x0000FFFF;
        short y = (msg->lParam & 0xFFFF0000) >> 16;

        bool resizeWidth = minimumWidth() != maximumWidth();
        bool resizeHeight = minimumHeight() != maximumHeight();
        if (resizeWidth) {
            //left border
            if (x >= winrect.left && x < winrect.left + borderWidth) {
                *result = HTLEFT;
            }
            //right border
            if (x < winrect.right && x >= winrect.right - borderWidth) {
                *result = HTRIGHT;
            }
        }
        if (resizeHeight) {
            //bottom border
            if (y < winrect.bottom && y >= winrect.bottom - borderWidth) {
                *result = HTBOTTOM;
            }
            //top border
            if (y >= winrect.top && y < winrect.top + borderWidth) {
                *result = HTTOP;
            }
        }
        if (resizeWidth && resizeHeight) {
            //bottom left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                    y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMLEFT;
            }
            //bottom right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                    y < winrect.bottom && y >= winrect.bottom - borderWidth)
            {
                *result = HTBOTTOMRIGHT;
            }
            //top left corner
            if (x >= winrect.left && x < winrect.left + borderWidth &&
                    y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPLEFT;
            }
            //top right corner
            if (x < winrect.right && x >= winrect.right - borderWidth &&
                    y >= winrect.top && y < winrect.top + borderWidth)
            {
                *result = HTTOPRIGHT;
            }
        }

        if (*result != 0)
            return true;

        QWidget *action = QApplication::widgetAt(QCursor::pos());
        if (action == this) {
            *result = HTCAPTION;
            return true;
        }
    }

    return false;
#else
    return QMainWindow::nativeEvent(eventType, message, result);
#endif // _WIN32
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
    m_bottomButtonGroup->move((width() - m_bottomButtonGroup->width()) / 2,
                              height() - m_bottomButtonGroup->height());
    m_gv->move(width() - m_gv->width(), height() - m_gv->height());
}

void MainWindow::toggleProtectedMode()
{
    m_protectedMode = !m_protectedMode;
    m_closeButton->setVisible(!m_protectedMode);
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

void MainWindow::quitAppAction(bool force)
{
    if (!m_protectedMode || force) {
        closeWindow();
    }
}
