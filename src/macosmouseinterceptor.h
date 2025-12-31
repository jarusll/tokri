#ifndef MACOSMOUSEINTERCEPTOR_H
#define MACOSMOUSEINTERCEPTOR_H

#include <QObject>
#include <CoreGraphics/CoreGraphics.h>
#include "horizontalshakedetector.h"

class MacOSMouseInterceptor : public QObject {
    Q_OBJECT
public:
    explicit MacOSMouseInterceptor(QObject *parent = nullptr);
    ~MacOSMouseInterceptor();

    bool start();
    void stop();

signals:
    void shakeDetected();

private:
    static CGEventRef callback(CGEventTapProxy,
                               CGEventType,
                               CGEventRef,
                               void *userInfo);

    void handleEvent(CGEventType type, CGEventRef event);

    CFMachPortRef eventTap = nullptr;
    CFRunLoopSourceRef runLoopSource = nullptr;

    bool mLeftButtonClicked = false;

    bool mHaveLastAbsoluteX = false;
    double mLastAbsoluteX = 0.0;

    HorizontalShakeDetector mShakeDetector;
};

#endif // MACOSMOUSEINTERCEPTOR_H
