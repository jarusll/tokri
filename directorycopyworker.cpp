#include "directorycopyworker.h"

#include "filenameprovider.h"

DirectoryCopyWorker::DirectoryCopyWorker(QObject *parent)
    : QObject{parent}
{}

void DirectoryCopyWorker::copyDirectory(const QString &src)
{
    qDebug() << "Copying Directory" << src;

    QString dst = FileNameProvider::nameFromPath(src);
    QDir().mkpath(dst); // FIXME emit mkdirFail

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
                // FIXME emit copyFail
                return;
            }
        }
    }
}

void DirectoryCopyWorker::copyFile(const QString &filePath)
{
    QFile file(filePath);
    file.copy(FileNameProvider::nameFromPath(filePath));
    // FIXME emit copyFail
}


