#include "copyworker.h"

#include "filenameprovider.h"

CopyWorker::CopyWorker(QObject *parent)
    : QObject{parent}
{}

void CopyWorker::copyDirectory(const QString &src)
{
    qDebug() << "Copying Directory" << src;

    QString dst = FileNameProvider::nameFromPath(src);
    bool makePath = QDir().mkpath(dst);
    if (makePath == false){
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
            QDir().mkpath(out);
        } else {
            QDir().mkpath(QFileInfo(out).path());
            if (!QFile::copy(it.filePath(), out)){
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


