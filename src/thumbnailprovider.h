#ifndef THUMBNAILPROVIDER_H
#define THUMBNAILPROVIDER_H

#include <QObject>
#include <QIcon>
#include <QSize>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QFileIconProvider>
#include <QCache>

class ThumbnailProvider : public QObject
{
    Q_OBJECT
public:
    explicit ThumbnailProvider(QObject *parent = nullptr);
    QIcon iconForFile(const QFileInfo &fi, const QSize &size) const;

private:
    QString makeKey(const QFileInfo &fi) const;
    QIcon textPreviewIcon(const QFileInfo &fi, const QSize &size) const;

    mutable QCache<QString, QIcon> cache{256};
    QMimeDatabase db;
    QFileIconProvider iconProvider;
};

#endif // THUMBNAILPROVIDER_H
