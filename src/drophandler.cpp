#include "drophandler.h"

#include "filepathprovider.h"
#include "textfile.h"

#include <QColorSpace>
#include <QDir>
#include <QDirIterator>
#include <QFileInfo>
#include <QImageReader>
#include <QImageWriter>
#include <QMimeDatabase>
#include <QScopedPointer>
#include <QUrl>

DropHandler::DropHandler(QObject *parent)
    : QObject{parent}
{}

void DropHandler::handleDrop(QMimeData *data)
{
    if (!data)
        return;

    QScopedPointer<QMimeData> guard(data);

    bool wrote = false;

    if (data->hasUrls()) {
        for (const QUrl &url : data->urls()) {
            if (url.isLocalFile()) {
                const QFileInfo info(url.toLocalFile());
                if (info.isDir())
                    wrote |= copyDirectory(info.absoluteFilePath());
                else if (info.isFile())
                    wrote |= copyFile(info.absoluteFilePath());
            } else {
                wrote |= saveUrl(url.toString());
            }
        }
    } else {
        for (const QString &format : data->formats()) {
            if (!format.startsWith("image/"))
                continue;
            if (!QImageReader::supportedMimeTypes().contains(format.toLatin1()))
                continue;

            const QByteArray bytes = data->data(format);
            if (bytes.isEmpty())
                continue;

            if (saveImageBytes(bytes, format)) {
                wrote = true;
                break;
            }
        }

        if (!wrote && data->hasImage()) {
            const QImage image = data->imageData().value<QImage>();
            if (!image.isNull())
                wrote = saveImage(image);
        }

        if (!wrote && data->hasText())
            wrote = saveText(data->text());

        if (!wrote && data->hasHtml())
            wrote = saveHtml(data->html());
    }

    if (wrote)
        emit changed();
}

bool DropHandler::copyFile(const QString &filePath)
{
    const QString dst = FilePathProvider::uniquePath(
        FilePathProvider::nameFromPath(filePath));

#if defined(Q_OS_MACOS)
    QFile in(filePath);
    QFile out(dst);

    if (!in.open(QIODevice::ReadOnly) ||
        !out.open(QIODevice::WriteOnly | QIODevice::Truncate) ||
        out.write(in.readAll()) == -1) {
        emit failed(filePath);
        return false;
    }
    return true;
#else
    if (!QFile::copy(filePath, dst)) {
        emit failed(filePath);
        return false;
    }
    return true;
#endif
}

bool DropHandler::copyDirectory(const QString &directory)
{
    const QString dstFinal = FilePathProvider::uniquePath(
        FilePathProvider::nameFromPath(directory));

#if defined(Q_OS_MACOS)
    const QString dst = dstFinal + ".progress";
#else
    const QString dst = dstFinal;
#endif

    if (!QDir().mkpath(dst)) {
        emit failed(dst);
        return false;
    }

    QDirIterator it(directory,
                    QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();

        const QString rel = QDir(directory).relativeFilePath(it.filePath());
        const QString out = dst + "/" + rel;

        if (it.fileInfo().isDir()) {
            if (!QDir().mkpath(out)) {
                emit failed(out);
                return false;
            }
        } else {
            QDir().mkpath(QFileInfo(out).path());
            if (!QFile::copy(it.filePath(), out)) {
                emit failed(out);
                return false;
            }
        }
    }

#if defined(Q_OS_MACOS)
    if (QFileInfo::exists(dstFinal))
        QDir(dstFinal).removeRecursively();

    QDir tmpDir = QFileInfo(dstFinal).dir();
    tmpDir.rename(QFileInfo(dst).fileName(), QFileInfo(dstFinal).fileName());
#endif

    return true;
}

bool DropHandler::saveImageBytes(const QByteArray &bytes, const QString &mimeType)
{
    QMimeDatabase db;
    QString suffix = db.mimeTypeForName(mimeType).preferredSuffix();
    if (suffix.isEmpty())
        suffix = "png";

    const QString path = FilePathProvider::uniquePath(
        FilePathProvider::nameWithPrefix("image") + "." + suffix);

    QFile out(path);
    if (!out.open(QIODevice::WriteOnly)) {
        emit failed(path);
        return false;
    }

    if (out.write(bytes) == -1) {
        emit failed(path);
        return false;
    }

    return true;
}

bool DropHandler::saveImage(const QImage &image)
{
    const QString path = FilePathProvider::uniquePath(
        FilePathProvider::nameWithPrefix("image") + ".png");

    QImage out = image;

#ifdef Q_OS_MACOS
    out.setColorSpace(QColorSpace());
    out = out.convertToFormat(QImage::Format_ARGB32);
#endif

    QImageWriter writer(path, "png");
    if (!writer.write(out)) {
        emit failed(path);
        return false;
    }

    return true;
}

bool DropHandler::saveUrl(const QString &url)
{
    TextFile file;
    file.setName(FilePathProvider::nameWithPrefix("link") + ".txt");
    file.setContent(url);
    return file.save();
}

bool DropHandler::saveText(const QString &text)
{
    TextFile file;
    file.setName(FilePathProvider::nameWithPrefix("text") + ".txt");
    file.setContent(text);
    return file.save();
}

bool DropHandler::saveHtml(const QString &html)
{
    TextFile file;
    file.setName(FilePathProvider::nameWithPrefix("text") + ".html");
    file.setContent(html);
    return file.save();
}
