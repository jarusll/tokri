#include "fstolistproxy.h"

FSToListProxy::FSToListProxy(QObject *parent)
    : QAbstractProxyModel{parent}
{}

QModelIndex FSToListProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid() || !mRoot.isValid() || !sourceModel())
        return QModelIndex();
    if (sourceIndex.parent() != mRoot)
        return QModelIndex();
    return index(sourceIndex.row(), sourceIndex.column(), QModelIndex());
}

QModelIndex FSToListProxy::mapToSource(const QModelIndex &proxyIndex) const
{
    if (!proxyIndex.isValid() || !mRoot.isValid())
        return QModelIndex();
    auto *src = sourceModel();
    return src->index(proxyIndex.row(), proxyIndex.column(), mRoot);
}

QModelIndex FSToListProxy::index(int row, int column, const QModelIndex &parent) const
{
    if (parent.isValid() || !mRoot.isValid())
        return QModelIndex();
    if (row < 0 || row >= rowCount() || column < 0 || column >= columnCount())
        return QModelIndex();
    QModelIndex src = sourceModel()->index(row, column, mRoot);
    if (!src.isValid())
        return QModelIndex();

    return createIndex(row, column, src.internalPointer());
}

QModelIndex FSToListProxy::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int FSToListProxy::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid() || !mRoot.isValid() || !sourceModel()){
        return 0;
    }
    return sourceModel()->rowCount(mRoot);
}

int FSToListProxy::columnCount(const QModelIndex &parent) const
{
    if (!mRoot.isValid() || !sourceModel())
        return 0;
    return sourceModel()->columnCount(mRoot);
}

QVariant FSToListProxy::data(const QModelIndex &idx, int role) const
{
    return sourceModel()->data(mapToSource(idx), role);
}

Qt::ItemFlags FSToListProxy::flags(const QModelIndex &idx) const
{
    return sourceModel()->flags(mapToSource(idx));
}

void FSToListProxy::setRootSourceIndex(const QModelIndex &srcRoot)
{
    beginResetModel();
    mRoot = QPersistentModelIndex(srcRoot);
    endResetModel();
}

QModelIndex FSToListProxy::rootSourceIndex() const
{
    return mRoot;
}
