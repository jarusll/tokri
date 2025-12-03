#ifndef DIRECTORYCOPYWORKER_H
#define DIRECTORYCOPYWORKER_H

#include <QObject>
#include <QDebug>
#include <QDir>
#include <QDirIterator>

class DirectoryCopyWorker : public QObject
{
    Q_OBJECT
public:
    explicit DirectoryCopyWorker(QObject *parent = nullptr);
    void copy(const QString &directory);

signals:
    void finished(bool status);
};

#endif // DIRECTORYCOPYWORKER_H
