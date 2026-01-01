#include "thumbnailprovider.h"

#include <QPixmap>

ThumbnailProvider::ThumbnailProvider(QObject *parent)
    : QObject{parent}
{}

QIcon ThumbnailProvider::iconForFile(const QFileInfo &fi, const QSize &size) const
{
    if (db.mimeTypeForFile(fi).name().startsWith("image/")) {
        QPixmap pm(fi.filePath());
        if (!pm.isNull()) {
            pm = pm.scaled(size, Qt::KeepAspectRatio, Qt::SmoothTransformation);
            return QIcon(pm);
        }
    }

    return iconProvider.icon(fi);
}
