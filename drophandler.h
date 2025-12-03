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
    bool dropText(const QString &text);
    bool dropDirectory(const QString &directory);

signals:
};

#endif // DROPHANDLER_H
