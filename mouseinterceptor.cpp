#include "mouseinterceptor.h"

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>

#include <QDebug>

MouseInterceptor::MouseInterceptor(QObject *parent)
    : QObject{parent}
{
    const char *dev = "/dev/input/event5"; // FIXME add in global var

    mEventsFd = ::open(dev, O_RDONLY | O_NONBLOCK);
    if (mEventsFd < 0) {
        // FIXME handle error (log, etc.)
        qDebug() << "failed to open";
        return;
    }

    mNotifier = new QSocketNotifier(mEventsFd, QSocketNotifier::Read, this);
    connect(mNotifier, &QSocketNotifier::activated,
            this, &MouseInterceptor::onEvents);
}

void MouseInterceptor::onEvents()
{
    // qDebug() << "Evdev ready";
    struct input_event buffer[16];

    while (true) {
        ssize_t n = ::read(mEventsFd, buffer, sizeof(buffer));
        if (n < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK)
                break;
            // FIXME error
            return;
        }
        if (n == 0)
            // FIXME error
            return;

        const size_t eventsRead = n / sizeof(struct input_event);
        // qDebug() << "Read" << eventsRead << "items";
        for (size_t i = 0; i < eventsRead; ++i) {
            const struct input_event &event = buffer[i];
            // qDebug() << "Event" << event.type << event.code << event.value;

            if (event.type != EV_REL && event.type != EV_ABS && event.type != EV_KEY){
                continue;
            }

            if (event.type == EV_KEY){
                if (event.code == BTN_LEFT){
                    if (event.value == 1){
                        mLeftClicked = true;
                    } else {
                        mLeftClicked = false;
                        mShake.reset();
                    }
                }
            }
            int dx = 0;

            // Relative mouse
            auto isEventRelative = event.type == EV_REL;
            auto isEventAbsolute = event.type == EV_ABS;
            auto isHorizontalAbsoluteMovement =
                (event.code == ABS_X || event.code == ABS_MT_POSITION_X);
            auto isHorizontalRelativeMovement = event.code == REL_X;

            if (isEventRelative && isHorizontalRelativeMovement) {
                qDebug() << "Relative";
                dx = event.value;
            }
            // Touchpad absolute → convert to relative
            else if (isEventAbsolute && isHorizontalAbsoluteMovement) {
                // qDebug() << "Absolute" << mHaveLastX << mLastX;
                if (mHaveLastX) {
                    dx = event.value - mLastX;
                }
                mLastX = event.value;
                mHaveLastX = true;
            }

            if (dx == 0) {
                continue;
            }

            uint64_t tsMs =
                (uint64_t)event.time.tv_sec * 1000ull +
                (uint64_t)event.time.tv_usec / 1000ull;

            if (mLeftClicked && mShake.feed(dx, tsMs)){
                emit shakeDetected();
                qDebug() << "Shaked";
            }
        }
    }
}
