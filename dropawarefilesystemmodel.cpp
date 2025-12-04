#include "dropawarefilesystemmodel.h"

#include <QBuffer>


DropAwareFileSystemModel::DropAwareFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{
    setReadOnly(false);
}

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

        // FIXME cleanup
        for (const auto &url: data->urls()){
            if (url.isLocalFile()){
                QFileInfo fileInfo(url.toLocalFile());
                if (fileInfo.isFile()){
                    // qDebug() << "Dropped File" << fileInfo;
                    // emit droppedFile(fileInfo.filePath());
                    fsUrls << url;
                    emit droppedFile(url.toLocalFile());
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
        // if (fsUrls.length() > 0){
        //     QMimeData *filesData = new QMimeData;
        //     filesData->setUrls(fsUrls);
        //     qDebug() << "Files" << fsUrls;
        //     bool baseHandled = QFileSystemModel::dropMimeData(
        //         filesData,
        //         action,
        //         row,
        //         column,
        //         parent
        //     );
        //     handled = baseHandled | handled;
        // }

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

    // DECISION - not to save html for now
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

QVariant DropAwareFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QFileSystemModel::data(index, role);
    }
    if (role == Qt::ToolTipRole){
        const auto path = filePath(index);
        QMimeDatabase mimeDb;
        auto mime = mimeDb.mimeTypeForFile(path);
        if (mime.inherits("text/plain")){
            QFile file(path);
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
                QVariant();

            QByteArray buf = file.read(1024);
            if (file.bytesAvailable() > 1024){
                buf.append("...");
            }
            return QString::fromUtf8(buf);
        } else if (mime.inherits("image/png")){
            // FIXME is this even needed?
            qDebug() << "Tooltip Image";
            QPixmap pix(path);
            QByteArray ba;
            QBuffer buffer(&ba);
            buffer.open(QIODevice::WriteOnly);
            pix.save(&buffer, "PNG");
            QString base64 = ba.toBase64();

            return "<img src=\"data:image/png;base64," + base64 + "\" width=\"256\"/>";
        } else {
            return QFileSystemModel::data(index, role);
        }
    }

    return QFileSystemModel::data(index, role);
}

QMimeData* DropAwareFileSystemModel::mimeData(const QModelIndexList &indexes) const
{
    QMimeData *mime = QFileSystemModel::mimeData(indexes);
    if (!mime)
        mime = new QMimeData;

    // FIXME multiple txt drags will be treated as files, do something?
    if (indexes.length() == 1){
        const QModelIndex idx = indexes.first();
        const QString path = filePath(idx);
        QMimeDatabase mimeDb;
        if (mimeDb.mimeTypeForFile(path).inherits("text/plain")){
            QFile f(path);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray bytes = f.readAll();
                mime->setData("text/plain", bytes);
            }
        }
    }
    return mime;
}
