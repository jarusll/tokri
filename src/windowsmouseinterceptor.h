#ifndef WINDOWSMOUSEINTERCEPTOR_H
#define WINDOWSMOUSEINTERCEPTOR_H

#include "horizontalshakedetector.h"

#include <Windows.h>
#include <QObject>
#include <QThread>
#include <QDebug>

class WindowsMouseInterceptor : public QThread
{
    Q_OBJECT
public:
    explicit WindowsMouseInterceptor(QObject *parent = nullptr);

    static WindowsMouseInterceptor* instance;

protected:
    void run() override;

signals:
    void shakeDetected();

private:
    static LRESULT CALLBACK MouseProc(int nCode, WPARAM wParam, LPARAM lParam);
    static HHOOK mouseHook;
    static int mLastAbsoluteX;
    static bool mLeftButtonClicked;
    static HorizontalShakeDetector mShake;
};

#endif // WINDOWSMOUSEINTERCEPTOR_H
