#ifndef MOUSEINTERCEPTOR_H
#define MOUSEINTERCEPTOR_H

#include <QDir>
#include <QObject>
#include <QSocketNotifier>

#include <fcntl.h>
#include <unistd.h>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <dirent.h>
#include <cstdio>
#include <cstring>

#include "horizontalshakedetector.h"

#ifndef BITS_PER_LONG
#define BITS_PER_LONG (sizeof(unsigned long) * 8)
#endif

static inline bool testBit(int bit, const unsigned long *array)
{
    const int index = bit / (sizeof(unsigned long) * 8);
    const int offset = bit % (sizeof(unsigned long) * 8);
    return array[index] & (1UL << offset);
}

class MouseInterceptor : public QObject
{
    Q_OBJECT
public:
    explicit MouseInterceptor(QObject *parent = nullptr);

    void onEvents(int fd);

    static QStringList collectRelAbsDevices()
    {
        QStringList out;

        QDir dir("/dev/input");
        const QStringList entries =
            dir.entryList(QStringList() << "event*",
                          QDir::Files | QDir::System);

        for (const QString &name : entries) {
            const QString path = dir.absoluteFilePath(name);

            int fd = ::open(path.toLocal8Bit().constData(),
                            O_RDONLY | O_CLOEXEC);
            if (fd < 0)
                continue;

            unsigned long evbits[(EV_MAX + BITS_PER_LONG) / BITS_PER_LONG] = {0};
            if (::ioctl(fd, EVIOCGBIT(0, sizeof(evbits)), evbits) < 0) {
                ::close(fd);
                continue;
            }

            const bool hasRel = testBit(EV_REL, evbits);
            const bool hasAbs = testBit(EV_ABS, evbits);

            if (hasRel || hasAbs)
                out << path;

            ::close(fd);
        }

        return out;
    }

signals:
    void shakeDetected();

private:
    int mCurrentEventsFd;
    bool mLeftButtonClicked = false;

    // For EV_ABS devices
    int mLastAbsoluteX = 0;
    bool mHaveLastAbsoluteX = false;

    HorizontalShakeDetector mShakeDetector;
};

#endif // MOUSEINTERCEPTOR_H
