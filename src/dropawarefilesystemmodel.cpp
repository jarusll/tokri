#include "dropawarefilesystemmodel.h"

#include "loghelpers.h"

#include <QApplication>
#include <QFile>
#include <QMimeDatabase>

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

    if (action == Qt::IgnoreAction) {
        qInfo().noquote() << threadTag() << "canDropMimeData action=IgnoreAction -> true";
        return true;
    }

    if (!data) {
        qWarning().noquote() << threadTag() << "canDropMimeData null data -> false";
        return false;
    }

    const bool ok = data->hasUrls()
           || data->hasImage()
           || data->hasText()
           || data->hasHtml();

    qInfo().noquote() << threadTag() << "canDropMimeData" << describeMimeData(data)
                      << "->" << ok;
    return ok;
}

bool DropAwareFileSystemModel::dropMimeData(const QMimeData *data,
                                            Qt::DropAction action,
                                            int row, int column,
                                            const QModelIndex &parent) {
    qInfo().noquote() << threadTag() << "dropMimeData ENTER action=" << action
                      << "row=" << row << "col=" << column
                      << "parentValid=" << parent.isValid();
    dumpMimeData(data);

    if (!canDropMimeData(data, action, row, column, parent)) {
        qInfo().noquote() << threadTag() << "dropMimeData not acceptable -> false";
        return false;
    }

    QMimeData *copy = cloneMimeData(data);
    qInfo().noquote() << threadTag() << "dropMimeData cloned formats=" << copy->formats();

    if (mDropReceiver) {
        copy->moveToThread(mDropReceiver->thread());
        qInfo().noquote() << threadTag() << "dropMimeData receiver="
                          << static_cast<const void *>(mDropReceiver)
                          << "recvThread="
                          << static_cast<const void *>(mDropReceiver->thread());
    } else {
        qWarning().noquote() << threadTag()
                             << "dropMimeData no drop receiver, clone leaks";
    }

    qInfo().noquote() << threadTag() << "dropMimeData emitted dropReceived";
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

    QString path;
    QString detected;
    bool injected = false;

    // FIXME multiple txt drags will be treated as files, do something?
    if (indexes.length() == 1){
        const QModelIndex idx = indexes.first();
        path = filePath(idx);

        QMimeDatabase mimeDb;
        detected = mimeDb.mimeTypeForFile(path).name();

        if (mimeDb.mimeTypeForFile(path).inherits("text/plain")){
            QFile f(path);
            if (f.open(QIODevice::ReadOnly | QIODevice::Text)) {
                QByteArray bytes = f.readAll();
                mime->setData("text/plain", bytes);
                injected = true;
                qInfo().noquote() << threadTag() << "mimeData injecting text/plain"
                                  << "bytes=" << bytes.size() << "path=" << path;
            } else {
                qWarning().noquote() << threadTag() << "mimeData failed to open text file"
                                     << "path=" << path << "err=" << f.errorString();
            }
        }
    }

    qInfo().noquote() << threadTag() << "mimeData drag-out"
                      << "indexes=" << indexes.size()
                      << "path=" << path
                      << "detectedMime=" << detected
                      << "injectedTextPlain=" << injected
                      << describeMimeData(mime);
    return mime;
}

Qt::DropActions DropAwareFileSystemModel::supportedDragActions() const {
    const Qt::DropActions actions =
        (QApplication::keyboardModifiers() & Qt::ControlModifier)
            ? Qt::CopyAction
            : Qt::MoveAction;
    qInfo().noquote() << threadTag() << "supportedDragActions ->" << actions
                      << "ctrl=" << bool(QApplication::keyboardModifiers() & Qt::ControlModifier);
    return actions;
}
