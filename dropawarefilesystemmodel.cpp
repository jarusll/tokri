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

    // TODO wtf is this?
    if (action == Qt::IgnoreAction)
        return true;

    if (data->hasUrls()){
        return true;
    } else if (data->hasText()){
        return true;
    }
    // FIXME
    // else if (data->hasImage()){
    //     qDebug() << "Dropping Image" << data->imageData();
    //     return true;
    // }
    return QFileSystemModel::canDropMimeData(
        data,
        action,
        row,
        column,
        parent
    );
}

bool DropAwareFileSystemModel::dropMimeData(const QMimeData *data,
                                            Qt::DropAction action,
                                            int row, int column,
                                            const QModelIndex &parent) {
    Q_UNUSED(row);
    Q_UNUSED(column);
    Q_UNUSED(parent);


    if (data->hasUrls()) {
        bool handled = false;
        QList<QUrl> fsUrls;
        QList<QUrl> directoryUrls;

        for (const auto &url: data->urls()){
            if (url.isLocalFile()){
                QFileInfo fileInfo(url.toLocalFile());
                if (fileInfo.isFile()){
                    // qDebug() << "Dropped File" << fileInfo;
                    // emit droppedFile(fileInfo.filePath());
                    fsUrls << url;
                } else {
                    // qDebug() << "Dropped Directory" << fileInfo;
                    directoryUrls << url;
                }
            } else {
                qDebug() << "Url" << url.url();
                emit droppedText(url.toString());
                handled = true;
            }
        }
        // emit dropped(data);
        // qDebug() << "Dirs" << directoryUrls;
        // qDebug() << "Files" << fsUrls;
        if (fsUrls.length() > 0){
            QMimeData *filesData = new QMimeData;
            filesData->setUrls(fsUrls);
            qDebug() << "Files" << fsUrls;
            bool baseHandled = QFileSystemModel::dropMimeData(
                filesData,
                action,
                row,
                column,
                parent
            );
            handled = baseHandled | handled;
        }

        if (directoryUrls.length() > 0){
            // QStringList directories;
            for (const auto &dir: directoryUrls){
                emit droppedDirectory(dir.toLocalFile());
                // directories << dir.toLocalFile();
            }
            // emit droppedDirectory(directories);
            handled = true;
            qDebug() << "Directories" << directoryUrls;
        }

        return handled;
    }

    if (data->hasText()) {
        emit droppedText(data->text());
        qDebug() << "Text" << data->text();
        return true;
    }

    // FIXME
    // if (data->hasImage()) {
    //     emit dropped(data);
    //     return true;
    // }
    return QFileSystemModel::dropMimeData(
        data,
        action,
        row,
        column,
        parent
    );
}
