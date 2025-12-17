#include "linuxmouseinterceptor.h"

#include <QDir>

MouseInterceptor::MouseInterceptor(QObject *parent)
    : QObject{parent}
{
    const QStringList absRelDevices = MouseInterceptor::collectRelAbsDevices();

    QVector<int> fds;
    for (const QString &path : absRelDevices) {
        int fd = ::open(path.toLocal8Bit().constData(),
                        O_RDONLY | O_CLOEXEC | O_NONBLOCK);
        if (fd < 0)
            continue;
        fds.append(fd);

        auto *notifier = new QSocketNotifier(fd,
                                             QSocketNotifier::Read,
                                             this);

        connect(notifier, &QSocketNotifier::activated,
                this, &MouseInterceptor::onEvents);
    }
}

void MouseInterceptor::onEvents(int fd)
{
    struct input_event buffer[17];

    while (true) {
        ssize_t n = ::read(fd, buffer, sizeof(buffer));
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
        for (size_t i = 0; i < eventsRead; ++i) {
            const struct input_event &event = buffer[i];

            if (event.type != EV_REL && event.type != EV_ABS && event.type != EV_KEY){
                continue;
            }

            if (event.type == EV_KEY){
                if (event.code == BTN_LEFT){
                    if (event.value == 1){
                        mLeftButtonClicked = true;
                    } else {
                        mLeftButtonClicked = false;
                        mShakeDetector.reset();
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
                dx = event.value;
            }
            // Touchpad absolute → convert to relative
            else if (isEventAbsolute && isHorizontalAbsoluteMovement) {
                if (mHaveLastAbsoluteX) {
                    dx = event.value - mLastAbsoluteX;
                }
                mLastAbsoluteX = event.value;
                mHaveLastAbsoluteX = true;
            }

            if (dx == 0) {
                continue;
            }

            if (!mLeftButtonClicked){
                continue;
            }

            uint64_t tsMs =
                (uint64_t)event.time.tv_sec * 1000ull +
                (uint64_t)event.time.tv_usec / 1000ull;

            if (mShakeDetector.feed(dx, tsMs)){
                emit shakeDetected();
            }
        }
    }
}


