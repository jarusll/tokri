#ifndef SORTFILTERPROXY_H
#define SORTFILTERPROXY_H

#include <QSortFilterProxyModel>

class SortFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit SortFilterProxy(QObject *parent = nullptr);
};

#endif // SORTFILTERPROXY_H
