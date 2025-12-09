#include "fstolistproxy.h"

FSToListProxy::FSToListProxy(QObject *parent)
    : QAbstractProxyModel{parent}
{}

QModelIndex FSToListProxy::mapFromSource(const QModelIndex &sourceIndex) const
{
    if (!sourceIndex.isValid())
        return QModelIndex();
    if (sourceIndex.parent() != mRoot)
        return QModelIndex();
    return createIndex(sourceIndex.row(), sourceIndex.column());
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
    return createIndex(row, column);
}

QModelIndex FSToListProxy::parent(const QModelIndex &child) const
{
    Q_UNUSED(child);
    return QModelIndex();
}

int FSToListProxy::rowCount(const QModelIndex &parent) const
{
    if (parent.isValid()){
        return 0;
    }
    if (!mRoot.isValid()){
        return 0;
    }
    return mRoot.model()->rowCount(mRoot);
}

int FSToListProxy::columnCount(const QModelIndex &parent) const
{
    if (!mRoot.isValid())
        return 0;
    return mRoot.model()->columnCount(mRoot);
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
    if (mRoot != srcRoot)
        mRoot = srcRoot;
}

QModelIndex FSToListProxy::rootSourceIndex() const
{
    return mRoot;
}
