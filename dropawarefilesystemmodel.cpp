#include "dropawarefilesystemmodel.h"

DropAwareFileSystemModel::DropAwareFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{}



Qt::ItemFlags DropAwareFileSystemModel::flags(const QModelIndex &index) const  {
    Qt::ItemFlags f = QFileSystemModel::flags(index);
    return f | Qt::ItemIsDropEnabled | Qt::ItemIsEditable;
}




bool DropAwareFileSystemModel::canDropMimeData(const QMimeData *data,
                                               Qt::DropAction action,
                                               int row, int column,
                                               const QModelIndex &parent) const {
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);
    if (action == Qt::IgnoreAction)
        return true;

    if (data->hasUrls()){
        qDebug() << "Dropping urls" << data->urls();
        return true;
    } else if (data->hasText()){
        qDebug() << "Dropping text" << data->text();
        return true;
    }
    // else if (data->hasImage()){
    //     qDebug() << "Dropping Image" << data->imageData();
    //     return true;
    // }
    else {
        return false;
    }
}

bool DropAwareFileSystemModel::dropMimeData(const QMimeData *data,
                                            Qt::DropAction action,
                                            int row, int column,
                                            const QModelIndex &parent) {
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);

    if (data->hasUrls()) {
        emit dropped(data);
        return true;
    }

    if (data->hasText()) {
        emit dropped(data);
        return true;
    }

    // if (data->hasImage()) {
    //     emit dropped(data);
    //     return true;
    // }
    return false;
}
