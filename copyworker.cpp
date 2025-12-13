#include "copyworker.h"

#include "filepathprovider.h"
#include "standardpaths.h"

#include <QBuffer>
#include <QImage>
#include <QImageReader>
#include <QMimeDatabase>
#include <QUuid>

CopyWorker::CopyWorker(QObject *parent)
    : QObject{parent}
{}

void CopyWorker::copyDirectory(const QString &src)
{
    QString dst = FilePathProvider::nameFromPath(src);
    bool makePath = QDir().mkpath(dst);
    if (makePath == false){
        // FIXME
        emit makePathFailed(dst);
    }

    QDirIterator it(src,
                    QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    while (it.hasNext()) {
        it.next();
        QString rel = QDir(src).relativeFilePath(it.filePath());
        QString out = dst + "/" + rel;

        if (it.fileInfo().isDir()) {
            // FIXME handle errors here
            QDir().mkpath(out);
        } else {
            // FIXME handle errors here
            QDir().mkpath(QFileInfo(out).path());
            if (!QFile::copy(it.filePath(), out)){
                // FIXME handle copy error
                emit copyFailed(out);
                return;
            }
        }
    }
}

void CopyWorker::copyFile(const QString &filePath)
{
    QFile file(filePath);
    bool copied = file.copy(FilePathProvider::nameFromPath(filePath));
    if (copied == false){
        emit copyFailed(filePath);
    }
}

void CopyWorker::saveImage(const QImage &image)
{
    QString fileName = FilePathProvider::nameWithPrefix("image");

    QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
    QString fullPath = QDir(rootPath).filePath(fileName);

    if (!image.save(fullPath)) {
        emit copyFailed(fullPath);
    }
}

static QByteArray mimeToFormat(const QString &mime){
    QMimeDatabase db;
    QMimeType mt = db.mimeTypeForName(mime);
    const QStringList exts = mt.suffixes();
    if (!exts.isEmpty()){
        return exts.first().toLatin1();
    }
    return QByteArray();
}

void CopyWorker::saveImageBytes(const QByteArray bytes,
                                const QString format)
{
    QBuffer buf;
    buf.setData(bytes);
    buf.open(QIODevice::ReadOnly);

    QImageReader reader(&buf, format.toLatin1());
    QImage img = reader.read();
    if (img.isNull())
        return;

    const QString ext = "." + mimeToFormat(format);
    qDebug() << "Ext" << ext;
    const QString fileName = FilePathProvider::nameWithPrefix("image") + ext;
    qDebug() << fileName;

    const QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
    const QString fullPath = QDir(rootPath).filePath(fileName);

    bool status = img.save(fullPath);
    if (!status){
        qDebug() << "Image save failed";
    }
}
