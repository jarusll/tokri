#ifndef FSTOLISTPROXY_H
#define FSTOLISTPROXY_H

#include <QAbstractProxyModel>
#include <QItemSelection>

class FSToListProxy : public QAbstractProxyModel
{
    Q_OBJECT
public:
    explicit FSToListProxy(QObject *parent = nullptr);

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const;
    QModelIndex mapToSource(const QModelIndex &proxyIndex) const;

    QModelIndex index(int row, int column,
                      const QModelIndex &parent = QModelIndex()) const;

    QModelIndex parent(const QModelIndex &child) const;

    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &idx, int role) const;
    Qt::ItemFlags flags(const QModelIndex &idx) const;

    // QFileSystemModel specific API
    void setRootSourceIndex(const QModelIndex &srcRoot);
    QModelIndex rootSourceIndex() const;
private:
    QPersistentModelIndex mRoot;
};

#endif // FSTOLISTPROXY_H
