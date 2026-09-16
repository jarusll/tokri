#include "linuxdragshakedetector.h"

#include "loghelpers.h"

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <dirent.h>
#include <fcntl.h>
#include <poll.h>
#include <unistd.h>
#include <linux/input.h>

#include <QDebug>

namespace {
constexpr int kPollTimeoutMs = 100;

bool has_bit(unsigned long *bits, int nr) { return bits[0] & (1UL << nr); }
}

LinuxDragShakeDetector::LinuxDragShakeDetector(QObject *parent)
    : QObject(parent)
{
    scanDevices();
    running = true;
    worker = std::thread(&LinuxDragShakeDetector::workerLoop, this);
}

LinuxDragShakeDetector::~LinuxDragShakeDetector()
{
    running = false;
    if (worker.joinable())
        worker.join();
    for (int fd : fds) {
        if (fd >= 0)
            ::close(fd);
    }
}

void LinuxDragShakeDetector::scanDevices()
{
    Logger &log = Logger::instance();
    log.push("scanDevices");

    DIR *dir = opendir("/dev/input");
    if (!dir) {
        qCritical() << "Failed to open /dev/input";
        exit(1);
    }

    log.log() << "REL devices advertising REL_X:";
    struct dirent *ent;
    while ((ent = readdir(dir)) != nullptr) {
        if (strncmp(ent->d_name, "event", 5) != 0)
            continue;

        char path[64];
        snprintf(path, sizeof(path), "/dev/input/%s", ent->d_name);

        int probe = ::open(path, O_RDONLY);
        if (probe < 0)
            continue;

        unsigned long evbits[EV_MAX / (8 * sizeof(unsigned long)) + 1] = {};
        if (ioctl(probe, EVIOCGBIT(0, sizeof(evbits)), evbits) < 0) {
            ::close(probe);
            continue;
        }

        bool relX = false;
        if (has_bit(evbits, EV_REL)) {
            unsigned long relbits[REL_MAX / (8 * sizeof(unsigned long)) + 1] = {};
            if (ioctl(probe, EVIOCGBIT(EV_REL, sizeof(relbits)), relbits) >= 0)
                relX = has_bit(relbits, REL_X);
        }

        if (relX) {
            log.log() << path;
            fds.push_back(probe);
        } else {
            ::close(probe);
        }
    }

    closedir(dir);

    if (fds.empty()) {
        qCritical() << "No input device with REL_X found";
        exit(1);
    }

    log.pop();
}

void LinuxDragShakeDetector::workerLoop()
{
    Logger &log = Logger::instance();
    log.push("workerLoop");

    std::vector<struct pollfd> pfds(fds.size());
    for (size_t i = 0; i < fds.size(); ++i) {
        pfds[i].fd = fds[i];
        pfds[i].events = POLLIN;
    }

    struct input_event buffer[64];

    while (running) {
        int n = poll(pfds.data(), pfds.size(), kPollTimeoutMs);
        if (n < 0) {
            if (errno == EINTR)
                continue;
            log.log() << "poll failed:" << strerror(errno);
            break;
        }
        if (n == 0)
            continue;

        for (size_t i = 0; i < pfds.size(); ++i) {
            if (!(pfds[i].revents & POLLIN))
                continue;

            ssize_t bytes = ::read(pfds[i].fd, buffer, sizeof(buffer));
            if (bytes <= 0) {
                log.log() << "failed to read input events from fd" << pfds[i].fd;
                continue;
            }

            size_t count = (size_t)bytes / sizeof(struct input_event);
            for (size_t j = 0; j < count; ++j) {
                const struct input_event &ev = buffer[j];
                if (ev.type == EV_KEY && ev.code == BTN_LEFT) {
                    if (ev.value == 1)
                        leftPressed = true;
                    else if (ev.value == 0) {
                        leftPressed = false;
                        detector.reset();
                    }
                    continue;
                }
                if (ev.type != EV_REL || ev.code != REL_X)
                    continue;

                if (leftPressed && detector.feed(ev.value))
                    emit shakeDetected();
            }
        }
    }

    log.pop();
}
