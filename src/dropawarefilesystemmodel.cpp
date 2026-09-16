#include "dropawarefilesystemmodel.h"

#include <QApplication>

namespace {

QMimeData *cloneMimeData(const QMimeData *src)
{
    auto *copy = new QMimeData;
    for (const QString &format : src->formats())
        copy->setData(format, src->data(format));
    return copy;
}

}

DropAwareFileSystemModel::DropAwareFileSystemModel(QObject *parent)
    : QFileSystemModel{parent}
{
    setReadOnly(false);
}

void DropAwareFileSystemModel::setDropReceiver(QObject *receiver)
{
    mDropReceiver = receiver;
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

    if (action == Qt::IgnoreAction)
        return true;

    if (!data)
        return false;

    return data->hasUrls()
           || data->hasImage()
           || data->hasText()
           || data->hasHtml();
}

bool DropAwareFileSystemModel::dropMimeData(const QMimeData *data,
                                            Qt::DropAction action,
                                            int row, int column,
                                            const QModelIndex &parent) {
    if (!canDropMimeData(data, action, row, column, parent))
        return false;

    QMimeData *copy = cloneMimeData(data);
    if (mDropReceiver)
        copy->moveToThread(mDropReceiver->thread());

    emit dropReceived(copy);
    return true;
}

QVariant DropAwareFileSystemModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid()){
        return QFileSystemModel::data(index, role);
    }

    if (role == Qt::ToolTipRole){
        return fileName(index);
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

Qt::DropActions DropAwareFileSystemModel::supportedDragActions() const {
    return (QApplication::keyboardModifiers() & Qt::ControlModifier)
               ? Qt::CopyAction
               : Qt::MoveAction;
}
