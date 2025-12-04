#ifndef DROPAWAREFILESYSTEMMODEL_H
#define DROPAWAREFILESYSTEMMODEL_H

#include <QFileSystemModel>
#include <QMimeData>
#include <QUrl>
#include <QMimeDatabase>

class DropAwareFileSystemModel : public QFileSystemModel
{
    Q_OBJECT
public:
    explicit DropAwareFileSystemModel(QObject *parent = nullptr);

    Qt::ItemFlags flags(const QModelIndex &index) const override;
    bool canDropMimeData(const QMimeData *data,
                         Qt::DropAction action,
                         int row, int column,
                         const QModelIndex &parent) const override;
    bool dropMimeData(const QMimeData *data,
                      Qt::DropAction action,
                      int row, int column,
                      const QModelIndex &parent) override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QMimeData* mimeData(const QModelIndexList &indexes) const override;

signals:
    void droppedText(const QString &text);
    void droppedFile(const QString &file);
    void droppedDirectory(const QString &directory);
};

#endif // DROPAWAREFILESYSTEMMODEL_H
