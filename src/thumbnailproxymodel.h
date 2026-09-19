#ifndef THUMBNAILPROXYMODEL_H
#define THUMBNAILPROXYMODEL_H

#include <QCache>
#include <QList>
#include <QPersistentModelIndex>
#include <QPixmap>
#include <QSet>
#include <QSortFilterProxyModel>
#include <QTimer>

class ThumbnailProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ThumbnailProxyModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const override;

public slots:
    void setVisibleCount(int count);

private:
    void requestThumbnail(const QModelIndex &index) const;
    void dispatchPendingRequests();

    QCache<QString, QPixmap> mCache;
    mutable QList<QPersistentModelIndex> mPendingRequests;
    mutable QTimer mDebounceTimer;
    QSet<QString> mInFlightRequests;
    QSet<QString> mFailedRequests;
    int mVisibleCount;
};

#endif // THUMBNAILPROXYMODEL_H
