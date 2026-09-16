#ifndef LINUXDRAGSHAKEDETECTOR_H
#define LINUXDRAGSHAKEDETECTOR_H

#include <QObject>

#include <atomic>
#include <thread>
#include <vector>

#include "shakedetector.h"

class LinuxDragShakeDetector : public QObject
{
    Q_OBJECT

public:
    explicit LinuxDragShakeDetector(QObject *parent = nullptr);
    ~LinuxDragShakeDetector() override;

signals:
    void shakeDetected();

private:
    void scanDevices();
    void workerLoop();

    std::vector<int> fds;
    std::thread worker;
    std::atomic<bool> running{false};
    bool leftPressed = false;

    ShakeDetector detector;
};

#endif // LINUXDRAGSHAKEDETECTOR_H