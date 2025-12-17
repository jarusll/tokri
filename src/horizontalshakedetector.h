#ifndef HORIZONTALSHAKEDETECTOR_H
#define HORIZONTALSHAKEDETECTOR_H

#include <QObject>

class HorizontalShakeDetector : public QObject
{
    Q_OBJECT
public:
    explicit HorizontalShakeDetector(QObject *parent = nullptr);

    bool feed(int dx, uint64_t tsMs);
    void reset();

    // FIXME maybe globals?
    uint64_t maxGapMs = 200;
    int flipThreshold = 3;

private:
    int lastDir  = 0;
    int flips    = 0;
    uint64_t lastTsMs = 0;
};

#endif // HORIZONTALSHAKEDETECTOR_H
