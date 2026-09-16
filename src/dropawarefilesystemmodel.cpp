#include "dropawarefilesystemmodel.h"

#include "loghelpers.h"

#include <QApplication>
#include <QFile>
#include <QMimeDatabase>

namespace {

QMimeData *cloneMimeData(const QMimeData *src)
{
    auto *copy = new QMimeData;
    if (src->hasImage())
        copy->setImageData(src->imageData());
    for (const QString &format : src->formats()) {
        if (copy->hasFormat(format))
            continue;
        copy->setData(format, src->data(format));
    }
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

    Logger &log = Logger::instance();
    log.push("canDropMimeData");

    if (action == Qt::IgnoreAction) {
        log.log() << "action=IgnoreAction -> true";
        log.pop();
        return true;
    }

    if (!data) {
        log.log() << "null data -> false";
        log.pop();
        return false;
    }

    const bool ok = data->hasUrls()
           || data->hasImage()
           || data->hasText()
           || data->hasHtml();

    log.log() << describeMimeData(data) << "->" << ok;

    log.pop();
    return ok;
}

bool DropAwareFileSystemModel::dropMimeData(const QMimeData *data,
                                            Qt::DropAction action,
                                            int row, int column,
                                            const QModelIndex &parent) {
    Logger &log = Logger::instance();
    log.push("dropMimeData");

    log.log() << "action=" << action
              << "row=" << row << "col=" << column
              << "parentValid=" << parent.isValid();
    dumpMimeData(data);

    if (!canDropMimeData(data, action, row, column, parent)) {
        log.log() << "not acceptable -> false";
        log.pop();
        return false;
    }

    QMimeData *copy = cloneMimeData(data);
    log.log() << "cloned formats=" << copy->formats();

    if (mDropReceiver) {
        copy->moveToThread(mDropReceiver->thread());
        log.log() << "receiver=" << static_cast<const void *>(mDropReceiver)
                  << "recvThread="
                  << static_cast<const void *>(mDropReceiver->thread());
    } else {
        log.log() << "no drop receiver, clone leaks";
    }

    log.log() << "emitted dropReceived";
    emit dropReceived(copy);

    log.pop();
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
    Logger &log = Logger::instance();
    log.push("mimeData");

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
                log.log() << "injecting text/plain"
                          << "bytes=" << bytes.size() << "path=" << path;
            } else {
                log.log() << "failed to open text file"
                          << "path=" << path << "err=" << f.errorString();
            }
        }
    }

    log.log() << "drag-out"
              << "indexes=" << indexes.size()
              << "path=" << path
              << "detectedMime=" << detected
              << "injectedTextPlain=" << injected
              << describeMimeData(mime);

    log.pop();
    return mime;
}

Qt::DropActions DropAwareFileSystemModel::supportedDragActions() const {
    Logger &log = Logger::instance();
    log.push("supportedDragActions");

    const Qt::DropActions actions =
        (QApplication::keyboardModifiers() & Qt::ControlModifier)
            ? Qt::CopyAction
            : Qt::MoveAction;
    log.log() << "->" << actions
              << "ctrl=" << bool(QApplication::keyboardModifiers() & Qt::ControlModifier);

    log.pop();
    return actions;
}
