#include "copyworker.h"

#include "filenameprovider.h"

CopyWorker::CopyWorker(QObject *parent)
    : QObject{parent}
{}

void CopyWorker::copyDirectory(const QString &src)
{
    QString dst = FileNameProvider::nameFromPath(src);
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
    bool copied = file.copy(FileNameProvider::nameFromPath(filePath));
    if (copied == false){
        emit copyFailed(filePath);
    }
}


