#include "thumbnailprovider.h"

#include <QPixmap>

ThumbnailProvider::ThumbnailProvider(QObject *parent)
    : QObject{parent}
{}

QString ThumbnailProvider::makeKey(const QFileInfo &fi) const
{
    return fi.filePath() + "|" +
           QString::number(fi.lastModified().toMSecsSinceEpoch());
}

QIcon ThumbnailProvider::iconForFile(const QFileInfo &fi, const QSize &size) const
{
    const QString key =
        fi.filePath() + "|" +
        QString::number(fi.lastModified().toMSecsSinceEpoch());

    if (auto *cached = cache.object(key)) {
        return *cached;
    }

    QIcon icon;

    if (db.mimeTypeForFile(fi).name().startsWith("image/")) {
        QPixmap pm(fi.filePath());
        if (!pm.isNull()) {
            pm = pm.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            icon = QIcon(pm);
        }
    }

    if (icon.isNull())
        icon = iconProvider.icon(fi);

    cache.insert(key, new QIcon(icon), 1);
    return icon;
}
