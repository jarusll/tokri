#ifndef SORTFILTERPROXY_H
#define SORTFILTERPROXY_H

#include <QSortFilterProxyModel>

class SortFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxy(QObject *parent = nullptr);
    bool lessThan(const QModelIndex &source_left, const QModelIndex &source_right) const;
};

#endif // SORTFILTERPROXY_H
