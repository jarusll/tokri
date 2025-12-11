#include "horizontalshakedetector.h"

#include <QDebug>

HorizontalShakeDetector::HorizontalShakeDetector(QObject *parent)
    : QObject{parent}
{}

bool HorizontalShakeDetector::feed(int dx, uint64_t tsMs)
{
    constexpr int minDx = 8;
    constexpr int maxDx = 100;

    // jitter
    if (std::abs(dx) < minDx)
        return false;

    // finger lifted / teleported on touchpad
    if (std::abs(dx) > maxDx) {
        return false;
    }

    qDebug() << dx;
    if (dx == 0)
        return false;

    int dir = (dx > 0) ? +1 : -1;

    // first movement or too slow → reset sequence
    if (lastDir == 0 || tsMs - lastTsMs > maxGapMs) {
        lastDir   = dir;
        flips     = 0;
        lastTsMs  = tsMs;
        return false;
    }

    // direction changed?
    if (dir != lastDir) {
        ++flips;
        lastDir   = dir;
        lastTsMs  = tsMs;

        if (flips >= flipThreshold) {
            reset();
            return true;
        }
    } else {
        // same direction, just refresh time
        lastTsMs = tsMs;
    }

    return false;
}

void HorizontalShakeDetector::reset()
{
    // qDebug() << "Reset";
    lastDir  = 0;
    flips    = 0;
    lastTsMs = 0;
}
