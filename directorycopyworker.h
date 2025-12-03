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
    void copying(const QString &directory);
    void copied(const QString &directory);
};

#endif // DIRECTORYCOPYWORKER_H
