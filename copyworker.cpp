#include "copyworker.h"

#include "filepathprovider.h"
#include "standardpaths.h"

#include <QBuffer>
#include <QImage>
#include <QImageReader>
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

void CopyWorker::saveImageBytes(QSharedPointer<QByteArray> bytes)
{
    QBuffer buf(bytes.data());
    buf.open(QIODevice::ReadOnly);
    QImageReader reader(&buf);
    QImage img = reader.read();
    auto filePath = FilePathProvider::nameWithPrefix("image");
    // TODO fix
    img.save(filePath);
}


