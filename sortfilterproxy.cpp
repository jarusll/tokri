#include "sortfilterproxy.h"

#include <QFileSystemModel>

SortFilterProxy::SortFilterProxy(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool SortFilterProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const auto leftFileInfo = left.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
    const auto rightFileInfo = right.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
    qDebug() << leftFileInfo.birthTime() << rightFileInfo.birthTime() << (leftFileInfo.birthTime() > rightFileInfo.birthTime());
    return leftFileInfo.birthTime() < rightFileInfo.birthTime();
}
