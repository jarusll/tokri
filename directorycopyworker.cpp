#include "directorycopyworker.h"

#include "filenameprovider.h"

DirectoryCopyWorker::DirectoryCopyWorker(QObject *parent)
    : QObject{parent}
{}

void DirectoryCopyWorker::copyDirectory(const QString &src)
{
    qDebug() << "Copying Directory" << src;

    QString dst = FileNameProvider::nameFromPath(src);
    QDir().mkpath(dst);

    QDirIterator it(src,
                    QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);

    emit copying(src);
    while (it.hasNext()) {
        it.next();
        QString rel = QDir(src).relativeFilePath(it.filePath());
        QString out = dst + "/" + rel;

        if (it.fileInfo().isDir()) {
            QDir().mkpath(out);
        } else {
            QDir().mkpath(QFileInfo(out).path());
            if (!QFile::copy(it.filePath(), out)){
                // FIXME
                return;
            }
        }
    }
    emit copied(src);
}

void DirectoryCopyWorker::copyFile(const QString &filePath)
{
    QFile file(filePath);
    file.copy(FileNameProvider::nameFromPath(filePath));
    // FIXME when it errors out
}


