#include "sortfilterproxy.h"
#include <rapidfuzz/fuzz.hpp>

#include <QFileSystemModel>

FSSortFilterProxy::FSSortFilterProxy(QObject *parent)
    : QSortFilterProxyModel{parent}
{}

bool FSSortFilterProxy::lessThan(const QModelIndex &left, const QModelIndex &right) const
{
    const auto leftFileInfo = left.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
    const auto rightFileInfo = right.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
    // qDebug() << leftFileInfo.birthTime() << rightFileInfo.birthTime() << (leftFileInfo.birthTime() > rightFileInfo.birthTime());
    if (mSearch.isEmpty()){
        return leftFileInfo.birthTime() < rightFileInfo.birthTime();
    }

    // FIXME - make dirs appear at the end because they cant be handled right now
    // const bool lIsDir = leftFileInfo.isDir();
    // const bool rIsDir = rightFileInfo.isDir();
    // if (lIsDir != rIsDir){

    // }

    qDebug() << leftFileInfo.fileName() << score(leftFileInfo.fileName()) << rightFileInfo.fileName() << score(rightFileInfo.fileName());
    return score(leftFileInfo.fileName()) < score(rightFileInfo.fileName());
}

bool FSSortFilterProxy::filterAcceptsRow(int row, const QModelIndex &parent) const
{
    if (mSearch.isEmpty())
        return true;

    auto *fs = qobject_cast<QFileSystemModel*>(sourceModel());
    if (!fs)
        return false;

    QModelIndex srcIndex = fs->index(row, 0, parent);
    if (!srcIndex.isValid())
        return false;

    // FIXME - please handle file for filtering
    // This can be solved if I use a filesystem => list proxy
    if (fs->isDir(srcIndex)){
        return true;
    }

    const QString name = fs->fileName(srcIndex);
    return score(name) > 10;
}

void FSSortFilterProxy::setSearch(const QString &string)
{
    if (mSearch != string){
        beginFilterChange();
        mSearch = string;
        endFilterChange();
    }
}

double FSSortFilterProxy::score(const QString &candidate) const
{
    const auto result = rapidfuzz::fuzz::ratio(mSearch.toStdString(), candidate.toLower().toStdString());
    return result;
}
