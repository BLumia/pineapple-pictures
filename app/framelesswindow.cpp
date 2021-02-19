#include "framelesswindow.h"

#include <QApplication>
#include <QVBoxLayout>

#ifdef _WIN32
#include <windows.h>
#endif // _WIN32

FramelessWindow::FramelessWindow(QWidget *parent)
    : QWidget(parent)
    , m_centralLayout(new QVBoxLayout(this))
{
    // We should use Qt::WindowMinMaxButtonsHint here but there is a bug in Qt
    // that will make pressing Meta+Up cause the app fullscreen under Windows,
    // so for now we only use the Qt::WindowMinimizeButtonHint flag here.
    // https://bugreports.qt.io/browse/QTBUG-91226
    this->setWindowFlags(Qt::Window | Qt::FramelessWindowHint | Qt::WindowMinimizeButtonHint);

    m_centralLayout->setMargin(0);
}

void FramelessWindow::setCentralWidget(QWidget *widget)
{
    if (m_centralWidget) {
        m_centralLayout->removeWidget(m_centralWidget);
        m_centralWidget->deleteLater();
    }

    m_centralLayout->addWidget(widget);
    m_centralWidget = widget;
}

bool FramelessWindow::nativeEvent(const QByteArray &eventType, void *message, long *result)
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
    return QWidget::nativeEvent(eventType, message, result);
#endif // _WIN32
}
