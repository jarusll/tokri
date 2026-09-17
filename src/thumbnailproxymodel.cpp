#include "thumbnailproxymodel.h"

#include <QFileSystemModel>
#include <QFutureWatcher>
#include <QIcon>
#include <QImageReader>
#include <QtConcurrent>

namespace {

QImage loadThumbnail(const QString &path, const QSize &target)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);

    const QSize size = reader.size();
    if (size.isValid())
        reader.setScaledSize(size.scaled(target, Qt::KeepAspectRatio));

    return reader.read();
}

}

ThumbnailProxyModel::ThumbnailProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    mCache.setMaxCost(30);
}

QVariant ThumbnailProxyModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DecorationRole || !index.isValid())
        return QSortFilterProxyModel::data(index, role);

    const QFileInfo fi =
        index.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
    if (!fi.isFile())
        return QSortFilterProxyModel::data(index, role);

    const QString key = fi.absoluteFilePath();

    if (const QPixmap *pm = mCache.object(key))
        return QIcon(*pm);
    if (mNonImages.contains(key))
        return QSortFilterProxyModel::data(index, role);

    if (!mPending.contains(key)) {
        if (!QImageReader(key).canRead()) {
            mNonImages.insert(key);
            return QSortFilterProxyModel::data(index, role);
        }

        mPending.insert(key);

        auto *self = const_cast<ThumbnailProxyModel *>(this);
        const QPersistentModelIndex pidx(index);

        auto future = QtConcurrent::run(
            [key] { return loadThumbnail(key, QSize(128, 128)); });

        auto *watcher = new QFutureWatcher<QImage>(self);
        connect(watcher, &QFutureWatcher<QImage>::finished, self,
                [self, watcher, key, pidx] {
                    const QImage img = watcher->result();
                    watcher->deleteLater();
                    self->mPending.remove(key);
                    if (img.isNull() || !pidx.isValid())
                        return;

                    self->mCache.insert(key, new QPixmap(QPixmap::fromImage(img)), 1);
                    emit self->dataChanged(pidx, pidx, {Qt::DecorationRole});
                });
        watcher->setFuture(future);
    }

    return QSortFilterProxyModel::data(index, role);
}
