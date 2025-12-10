#ifndef MOUSEINTERCEPTOR_H
#define MOUSEINTERCEPTOR_H

#include <QObject>
#include <QSocketNotifier>
#include "horizontalshakedetector.h"

class MouseInterceptor : public QObject
{
    Q_OBJECT
public:
    explicit MouseInterceptor(QObject *parent = nullptr);

    void onEvents();

signals:
    void shakeDetected();

private:
    int mEventsFd;
    int mLastX = 0;
    bool mHaveLastX = false;
    bool mLeftClicked = false;
    QSocketNotifier *mNotifier;
    HorizontalShakeDetector mShake;
};

#endif // MOUSEINTERCEPTOR_H
