#include "windowsmouseinterceptor.h"

int WindowsMouseInterceptor::mLastAbsoluteX = 0;
bool WindowsMouseInterceptor::mLeftButtonClicked = false;
HHOOK WindowsMouseInterceptor::mouseHook = nullptr;
HorizontalShakeDetector WindowsMouseInterceptor::mShake;
WindowsMouseInterceptor* WindowsMouseInterceptor::instance = nullptr;

WindowsMouseInterceptor::WindowsMouseInterceptor(QObject *parent)
    : QThread{parent}
{
    instance = this;
}

void WindowsMouseInterceptor::run()
{
    mouseHook = SetWindowsHookEx(
        WH_MOUSE_LL,
        MouseProc,
        nullptr,
        0
        );

    if (!mouseHook) {
        return;
    }

    // Message loop (required for WH_MOUSE_LL)
    exec();

    UnhookWindowsHookEx(mouseHook);
    mouseHook = nullptr;
}

LRESULT WindowsMouseInterceptor::MouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0) {
        MSLLHOOKSTRUCT* hookStruct = (MSLLHOOKSTRUCT*)lParam;

        switch (wParam) {
        case WM_MOUSEMOVE: {
            if (!mLeftButtonClicked){
                return CallNextHookEx(mouseHook, nCode, wParam, lParam);
            }
            int dx = 0;
            if (mLastAbsoluteX != 0) {
                dx = hookStruct->pt.x - mLastAbsoluteX;
            }
            mLastAbsoluteX = hookStruct->pt.x;

            if (dx == 0){
                return CallNextHookEx(mouseHook, nCode, wParam, lParam);
            } else {
                if (instance) {
                    if (mShake.feed(dx, (uint64_t)hookStruct->time)){
                        qDebug() << "Shake detected";
                        emit instance->shakeDetected();
                    }
                }
            }
            break;
        }

        case WM_LBUTTONDOWN: {
            mLeftButtonClicked = true;
            break;
        }

        case WM_LBUTTONUP: {
            mLeftButtonClicked = false;
            mLastAbsoluteX = 0;
            break;
        }}
    }
    return CallNextHookEx(mouseHook, nCode, wParam, lParam);
}
