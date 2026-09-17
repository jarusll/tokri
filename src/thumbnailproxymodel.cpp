#include "thumbnailproxymodel.h"

#include "textpreview.h"
#include "thumbnaillayout.h"

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

constexpr int TotalCacheBytes = 32 * 1024 * 1024;
constexpr int WindowMs = 50;
constexpr int MaxPendingRequests = 128;

const QSize ThumbnailSize{ThumbnailLayout::ContentSide,
                          ThumbnailLayout::ContentSide};
PreviewStyle themePreviewStyle;

QImage loadImage(const QString &path, const QSize &target, qreal dpr)
{
    QImageReader reader(path);
    reader.setAutoTransform(true);

    const QSize device = target * dpr;
    const QSize size = reader.size();
    if (size.isValid())
        reader.setScaledSize(size.scaled(device, Qt::KeepAspectRatio));

    QImage image = reader.read();
    if (image.isNull())
        return image;

    if (!size.isValid())
        image = image.scaled(device, Qt::KeepAspectRatio,
                             Qt::SmoothTransformation);

    image.setDevicePixelRatio(dpr);
    return image;
}

QImage loadPreview(const QString &path)
{
    const QMimeType mime =
        QMimeDatabase().mimeTypeForFile(path, QMimeDatabase::MatchContent);

    if (mime.name().startsWith(QLatin1String("image/")))
        return loadImage(path, ThumbnailSize, themePreviewStyle.dpr);
    if (isTextLike(mime))
        return renderTextPreview(path, ThumbnailSize, themePreviewStyle);

    return {};
}

PreviewStyle makeTextPreviewStyle()
{
    const QPalette pal = QGuiApplication::palette();

    PreviewStyle style;
    style.font = QFontDatabase::systemFont(QFontDatabase::FixedFont);
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
    mCache.setMaxCost(TotalCacheBytes);
    themePreviewStyle = makeTextPreviewStyle();

    mDebounceTimer.setSingleShot(true);
    mDebounceTimer.setInterval(WindowMs);
    connect(&mDebounceTimer, &QTimer::timeout, this,
            &ThumbnailProxyModel::dispatchPendingRequests);
}

QVariant ThumbnailProxyModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()
        || (role != Qt::DecorationRole
            && role != ThumbnailLayout::ThumbnailPixmapRole))
        return QSortFilterProxyModel::data(index, role);

    const QFileInfo fi =
        index.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
    if (!fi.isFile())
        return QSortFilterProxyModel::data(index, role);

    const QString key = fi.absoluteFilePath();

    if (const QPixmap *pm = mCache.object(key)) {
        if (role == ThumbnailLayout::ThumbnailPixmapRole)
            return QVariant::fromValue(*pm);
        return QIcon(*pm);
    }
    if (mFailedRequests.contains(key) || mInFlightRequests.contains(key)
        || mPendingRequests.contains(QPersistentModelIndex(index)))
        return QSortFilterProxyModel::data(index, role);

    requestThumbnail(index);

    return QSortFilterProxyModel::data(index, role);
}

void ThumbnailProxyModel::requestThumbnail(const QModelIndex &index) const
{
    mPendingRequests.append(QPersistentModelIndex(index));
    while (mPendingRequests.size() > MaxPendingRequests)
        mPendingRequests.removeFirst();

    mDebounceTimer.start();
}

void ThumbnailProxyModel::dispatchPendingRequests()
{
    const int limit = QThread::idealThreadCount();

    while (!mPendingRequests.isEmpty() && mInFlightRequests.size() < limit) {
        const QPersistentModelIndex pidx = mPendingRequests.takeLast();
        if (!pidx.isValid())
            continue;

        const QString key = pathOf(pidx);
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
                            emit dataChanged(
                                pidx, pidx,
                                {Qt::DecorationRole,
                                 ThumbnailLayout::ThumbnailPixmapRole});
                    }

                    if (!mPendingRequests.isEmpty() && !mDebounceTimer.isActive())
                        dispatchPendingRequests();
                });

        watcher->setFuture(QtConcurrent::run(
            [key] { return loadPreview(key); }));
    }
}
