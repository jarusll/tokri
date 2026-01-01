#ifndef COPYWORKER_H
#define COPYWORKER_H

#include <QObject>
#include <QDir>
#include <QDirIterator>

class CopyWorker : public QObject
{
    Q_OBJECT
public:
    explicit CopyWorker(QObject *parent = nullptr);

    void copyDirectory(const QString &directory);
    void copyFile(const QString &filePath);
    void saveImage(const QImage &image);
    void saveImageBytes(
        const QByteArray bytes,
        const QString format);

signals:
    void makePathFailed(QString);
    void copyFailed(QString);
    void copySuccess(QString);
};

#endif // COPYWORKER_H
