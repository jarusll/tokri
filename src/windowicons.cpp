#include "windowicons.h"

#include "themeprovider.h"

#include <QApplication>
#include <QIcon>
#include <QImage>
#include <QPainter>
#include <QPixmap>
#include <QWidget>
#include <QWindow>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

namespace {

#ifndef Q_OS_MACOS
QIcon themedWindowIcon()
{
    QPixmap pm(":/tray.png");
    if (ThemeProvider::isDark())
        return QIcon(pm);

    QPixmap dark(pm.size());
    dark.fill(Qt::transparent);
    QPainter p(&dark);
    p.drawPixmap(0, 0, pm);
    p.setCompositionMode(QPainter::CompositionMode_SourceIn);
    p.fillRect(dark.rect(), QColor("#141216"));
    p.end();
    return QIcon(dark);
}
#endif

#ifdef Q_OS_WIN
QIcon appTaskbarIcon()
{
    QIcon ico(":/net.surajyadav.Tokri.ico");
    return ico.isNull() ? themedWindowIcon() : ico;
}

void applyWindowsWindowIcons(QWidget &window)
{
    QWindow *handle = window.windowHandle();
    if (!handle)
        return; // Native window not created yet; applied again after show().

    HWND hwnd = (HWND)handle->winId();
    if (!hwnd)
        return;

    static HICON previousSmall = nullptr;
    static HICON previousBig = nullptr;

    HICON hSmall = themedWindowIcon().pixmap(16, 16).toImage().toHICON();
    HICON hBig = appTaskbarIcon().pixmap(256, 256).toImage().toHICON();

    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hSmall);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hBig);

    if (previousSmall)
        DestroyIcon(previousSmall);
    if (previousBig)
        DestroyIcon(previousBig);
    previousSmall = hSmall;
    previousBig = hBig;
}
#endif

} // namespace

namespace WindowIcons {

void apply(QApplication &app, QWidget &window)
{
#if defined(Q_OS_WIN)
    app.setWindowIcon(themedWindowIcon());
    applyWindowsWindowIcons(window);
#elif defined(Q_OS_MACOS)
    // No-op: the icon is supplied by the bundle's Tokri.icns.
    Q_UNUSED(app);
    Q_UNUSED(window);
#else
    app.setWindowIcon(themedWindowIcon());
    Q_UNUSED(window);
#endif
}

} // namespace WindowIcons
