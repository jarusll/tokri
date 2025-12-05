#ifndef SORTFILTERPROXY_H
#define SORTFILTERPROXY_H

#include <QSortFilterProxyModel>

class FSSortFilterProxy : public QSortFilterProxyModel
{
    Q_OBJECT
public:
    explicit FSSortFilterProxy(QObject *parent = nullptr);
    bool lessThan(const QModelIndex &left, const QModelIndex &right) const;
    bool filterAcceptsRow(int row, const QModelIndex &parent) const;

    void setSearch(const QString &string);

private:
    double score(const QString &candidate) const;
    QString mSearch;
};

#endif // SORTFILTERPROXY_H
