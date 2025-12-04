#ifndef COPYWORKER_H
#define COPYWORKER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

class CopyWorker : public QObject
{
    Q_OBJECT
public:
    explicit CopyWorker(QObject *parent = nullptr);
    void copyDirectory(const QString &directory);
    void copyFile(const QString &filePath);

signals:
};

#endif // COPYWORKER_H
