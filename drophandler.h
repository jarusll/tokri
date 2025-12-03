#ifndef DROPHANDLER_H
#define DROPHANDLER_H

#include <QMimeData>
#include <QObject>
#include <QUrl>
#include <QFile>

class DropHandler : public QObject
{
    Q_OBJECT
public:
    explicit DropHandler(QObject *parent = nullptr);
    bool drop(const QMimeData *data);

signals:
};

#endif // DROPHANDLER_H
