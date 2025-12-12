#include "dropawarefilesystemmodel.h"

#include "filepathprovider.h"

#include <QBuffer>
#include <QUuid>

bool isValidHttpUrl(const QString &s)
{
    QUrl u(s, QUrl::StrictMode);
    return u.isValid()
       && !u.scheme().isEmpty()
       && (u.scheme() == "http" || u.scheme() == "https")
       && !u.host().isEmpty();
}

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

    if (data->hasImage()){
        return true;
    } else if (data->hasUrls()){
        return true;
    } else if (data->hasText()){
        return true;
    }

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

    for (const QString &fmt : data->formats()) {
        if (fmt.startsWith("image/")) {
            QByteArray bytes = data->data(fmt);
            auto ptr = QSharedPointer<QByteArray>::create(bytes);
            emit droppedImageBytes(ptr);
            return true;
        }
    }

    if (data->hasImage()) {
        const auto image = data->imageData().value<QImage>();
        emit droppedImage(image);
        return true;
    }


    if (data->hasUrls()) {
        bool handled = false;

        // FIXME cleanup
        for (const auto &url: data->urls()){
            if (url.isLocalFile()){
                QFileInfo fileInfo(url.toLocalFile());
                if (fileInfo.isFile()){
                    emit droppedFile(url.toLocalFile());
                } else {
                    emit droppedDirectory(url.toLocalFile());
                }
            } else {
                emit droppedUrl(url.toString());
                handled = true;
            }
        }

        return handled;
    }

    if (data->hasText()) {
        if (isValidHttpUrl( data->text())){
            emit droppedUrl(data->text());
            return true;
        } else {
            emit droppedText(data->text());
            return true;
        }
    }

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
        return fileName(index);
    }

    // if (role == Qt::ToolTipRole){
    //     const auto path = filePath(index);
    //     QMimeDatabase mimeDb;
    //     auto mime = mimeDb.mimeTypeForFile(path);
    //     if (mime.inherits("text/plain")){
    //         QFile file(path);
    //         if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    //             QVariant();

    //         QByteArray buf = file.read(1024);
    //         if (file.bytesAvailable() > 1024){
    //             buf.append("...");
    //         }
    //         return QString::fromUtf8(buf);
    //     } else if (mime.inherits("image/png")){
    //         // FIXME is this even needed?
    //         QPixmap pix(path);
    //         QByteArray ba;
    //         QBuffer buffer(&ba);
    //         buffer.open(QIODevice::WriteOnly);
    //         pix.save(&buffer, "PNG");
    //         QString base64 = ba.toBase64();

    //         return "<img src=\"data:image/png;base64," + base64 + "\" width=\"256\"/>";
    //     } else {
    //         return QFileSystemModel::data(index, role);
    //     }
    // }

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
