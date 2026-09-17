#ifndef THUMBNAILPROXYMODEL_H
#define THUMBNAILPROXYMODEL_H

#include <QCache>
#include <QPixmap>
#include <QSet>
#include <QSortFilterProxyModel>

class ThumbnailProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

public:
    explicit ThumbnailProxyModel(QObject *parent = nullptr);
    QVariant data(const QModelIndex &index, int role) const override;

private:
    mutable QCache<QString, QPixmap> mCache;
    mutable QSet<QString> mPending;
    mutable QSet<QString> mNonImages;
};

#endif // THUMBNAILPROXYMODEL_H
