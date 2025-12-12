#ifndef DROPHANDLER_H
#define DROPHANDLER_H

#include <QMimeData>
#include <QObject>
#include <QUrl>
#include <QFile>

class TextDropHandler : public QObject
{
    Q_OBJECT
public:
    explicit TextDropHandler(QObject *parent = nullptr);
    bool handleTextDrop(const QString &text);
    bool handleUrlDrop(const QString &urlStr);
};

#endif // DROPHANDLER_H
