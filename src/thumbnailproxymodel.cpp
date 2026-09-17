#include "thumbnailproxymodel.h"

#include "textpreview.h"

#include <QApplication>
#include <QFileSystemModel>
#include <QFontDatabase>
#include <QFutureWatcher>
#include <QGuiApplication>
#include <QIcon>
#include <QImageReader>
#include <QMimeDatabase>
#include <QMimeType>
#include <QPalette>
#include <QThread>
#include <QtConcurrent>

namespace {

constexpr int kCacheBytes = 32 * 1024 * 1024;
constexpr int kWindowMs = 50;
constexpr int kMaxPending = 128;

QImage loadImage(const QString &path, const QSize &target)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);

    const QSize size = reader.size();
    if (size.isValid())
        reader.setScaledSize(size.scaled(target, Qt::KeepAspectRatio));

    return reader.read();
}

QImage loadPreview(const QString &path, const QSize &target,
                   const PreviewStyle &style)
{
    const QMimeType mime =
        QMimeDatabase().mimeTypeForFile(path, QMimeDatabase::MatchContent);

    if (mime.name().startsWith(QLatin1String("image/")))
        return loadImage(path, target);
    if (isTextLike(mime))
        return renderTextPreview(path, target, style);

    return {};
}

PreviewStyle makeStyle()
{
    const QPalette pal = QGuiApplication::palette();

    PreviewStyle style;
    style.font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
    style.background = pal.color(QPalette::Button);
    style.border = pal.color(QPalette::Mid);
    style.foreground = pal.color(QPalette::Text);
    style.dpr = qApp->devicePixelRatio();
    return style;
}

QString pathOf(const QPersistentModelIndex &pidx)
{
    return pidx.data(QFileSystemModel::FileInfoRole)
        .value<QFileInfo>()
        .absoluteFilePath();
}

}

ThumbnailProxyModel::ThumbnailProxyModel(QObject *parent)
    : QSortFilterProxyModel(parent)
{
    mCache.setMaxCost(kCacheBytes);

    mDebounceTimer.setSingleShot(true);
    mDebounceTimer.setInterval(kWindowMs);
    connect(&mDebounceTimer, &QTimer::timeout, this,
            &ThumbnailProxyModel::dispatchPendingRequests);
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
    if (mFailedRequests.contains(key) || mInFlightRequests.contains(key)
        || mPendingRequests.contains(QPersistentModelIndex(index)))
        return QSortFilterProxyModel::data(index, role);

    requestThumbnail(index);

    return QSortFilterProxyModel::data(index, role);
}

void ThumbnailProxyModel::requestThumbnail(const QModelIndex &index) const
{
    mPendingRequests.append(QPersistentModelIndex(index));
    while (mPendingRequests.size() > kMaxPending)
        mPendingRequests.removeFirst();

    mDebounceTimer.start();
}

void ThumbnailProxyModel::dispatchPendingRequests()
{
    const int limit = QThread::idealThreadCount();
    const PreviewStyle style = makeStyle();

    while (!mPendingRequests.isEmpty() && mInFlightRequests.size() < limit) {
        const QPersistentModelIndex pidx = mPendingRequests.takeLast();
        if (!pidx.isValid())
            continue;

        const QString key = pathOf(pidx);
        if (mFailedRequests.contains(key) || mInFlightRequests.contains(key))
            continue;

        mInFlightRequests.insert(key);

        auto *watcher = new QFutureWatcher<QImage>(this);
        connect(watcher, &QFutureWatcher<QImage>::finished, this,
                [this, watcher, key, pidx] {
                    const QImage img = watcher->result();
                    watcher->deleteLater();

                    mInFlightRequests.remove(key);

                    if (img.isNull()) {
                        mFailedRequests.insert(key);
                    } else {
                        mCache.insert(key, new QPixmap(QPixmap::fromImage(img)),
                                      int(img.sizeInBytes()));
                        if (pidx.isValid())
                            emit dataChanged(pidx, pidx,
                                             {Qt::DecorationRole});
                    }

                    if (!mPendingRequests.isEmpty() && !mDebounceTimer.isActive())
                        dispatchPendingRequests();
                });

        watcher->setFuture(QtConcurrent::run(
            [key, style] { return loadPreview(key, QSize(128, 128), style); }));
    }
}
