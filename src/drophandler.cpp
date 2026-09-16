#include "drophandler.h"

#include "filepathprovider.h"
#include "loghelpers.h"
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
    qInfo().noquote() << threadTag() << "handleDrop ENTER";
    if (!data) {
        qWarning().noquote() << threadTag() << "handleDrop null data";
        return;
    }

    QScopedPointer<QMimeData> guard(data);
    dumpMimeData(data);

    bool wrote = false;

    if (data->hasUrls()) {
        const QList<QUrl> urls = data->urls();
        qInfo().noquote() << threadTag() << "branch=urls count=" << urls.size();

        for (const QUrl &url : urls) {
            const bool local = url.isLocalFile();
            const QFileInfo info(local ? url.toLocalFile() : QString());

            qInfo().noquote() << threadTag() << "url=" << url.toString()
                              << "local=" << local
                              << "isDir=" << (local && info.isDir())
                              << "isFile=" << (local && info.isFile());

            if (local) {
                if (info.isDir())
                    wrote |= copyDirectory(info.absoluteFilePath());
                else if (info.isFile())
                    wrote |= copyFile(info.absoluteFilePath());
                else
                    qWarning().noquote() << threadTag()
                                         << "url is local but neither file nor dir, skipped";
            } else {
                wrote |= saveUrl(url.toString());
            }
        }
    } else {
        for (const QString &format : data->formats()) {
            if (!format.startsWith("image/"))
                continue;
            if (!QImageReader::supportedMimeTypes().contains(format.toLatin1())) {
                qInfo().noquote() << threadTag() << "skipping unsupported image format="
                                  << format;
                continue;
            }

            const QByteArray bytes = data->data(format);
            if (bytes.isEmpty()) {
                qInfo().noquote() << threadTag() << "empty image bytes for format=" << format;
                continue;
            }

            qInfo().noquote() << threadTag() << "branch=image-bytes mime=" << format
                              << "bytes=" << bytes.size();
            if (saveImageBytes(bytes, format)) {
                wrote = true;
                break;
            }
        }

        if (!wrote && data->hasImage()) {
            const QImage image = data->imageData().value<QImage>();
            qInfo().noquote() << threadTag() << "branch=image-data"
                              << "null=" << image.isNull()
                              << "size=" << image.width() << "x" << image.height();
            if (!image.isNull())
                wrote = saveImage(image);
        }

        if (!wrote && data->hasText()) {
            const QString text = data->text();
            qInfo().noquote() << threadTag() << "branch=text chars=" << text.size();
            wrote = saveText(text);
        }

        if (!wrote && data->hasHtml()) {
            const QString html = data->html();
            qInfo().noquote() << threadTag() << "branch=html chars=" << html.size();
            wrote = saveHtml(html);
        }
    }

    qInfo().noquote() << threadTag() << "handleDrop wrote=" << wrote;
    if (wrote) {
        qInfo().noquote() << threadTag() << "emit changed";
        emit changed();
    } else {
        qWarning().noquote() << threadTag() << "handleDrop stored nothing";
    }
}

bool DropHandler::copyFile(const QString &filePath)
{
    const QString dst = FilePathProvider::uniquePath(
        FilePathProvider::nameFromPath(filePath));

    bool ok = false;

#if defined(Q_OS_MACOS)
    QFile in(filePath);
    QFile out(dst);

    if (!in.open(QIODevice::ReadOnly) ||
        !out.open(QIODevice::WriteOnly | QIODevice::Truncate) ||
        out.write(in.readAll()) == -1) {
        emit failed(filePath);
        ok = false;
    } else {
        ok = true;
    }
#else
    if (!QFile::copy(filePath, dst)) {
        emit failed(filePath);
        ok = false;
    } else {
        ok = true;
    }
#endif

    qInfo().noquote() << threadTag() << "copyFile src=" << filePath
                      << "dst=" << dst << "ok=" << ok;
    return ok;
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

    qInfo().noquote() << threadTag() << "copyDirectory src=" << directory
                      << "dst=" << dst << "dstFinal=" << dstFinal;

    if (!QDir().mkpath(dst)) {
        qWarning().noquote() << threadTag() << "copyDirectory mkpath failed dst=" << dst;
        emit failed(dst);
        return false;
    }

    int files = 0;
    QDirIterator it(directory,
                    QDir::Files | QDir::Dirs | QDir::NoDotAndDotDot,
                    QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();

        const QString rel = QDir(directory).relativeFilePath(it.filePath());
        const QString out = dst + "/" + rel;

        if (it.fileInfo().isDir()) {
            if (!QDir().mkpath(out)) {
                qWarning().noquote() << threadTag()
                                     << "copyDirectory subdir mkpath failed out=" << out;
                emit failed(out);
                return false;
            }
        } else {
            QDir().mkpath(QFileInfo(out).path());
            if (!QFile::copy(it.filePath(), out)) {
                qWarning().noquote() << threadTag()
                                     << "copyDirectory file copy failed src=" << it.filePath()
                                     << "out=" << out;
                emit failed(out);
                return false;
            }
            ++files;
        }
    }

#if defined(Q_OS_MACOS)
    if (QFileInfo::exists(dstFinal))
        QDir(dstFinal).removeRecursively();

    QDir tmpDir = QFileInfo(dstFinal).dir();
    if (!tmpDir.rename(QFileInfo(dst).fileName(), QFileInfo(dstFinal).fileName())) {
        qWarning().noquote() << threadTag() << "copyDirectory final rename failed"
                             << "from=" << dst << "to=" << dstFinal;
    }
#endif

    qInfo().noquote() << threadTag() << "copyDirectory ok files=" << files
                      << "dstFinal=" << dstFinal;
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
        qWarning().noquote() << threadTag() << "saveImageBytes open failed path=" << path
                             << "err=" << out.errorString();
        emit failed(path);
        return false;
    }

    if (out.write(bytes) == -1) {
        qWarning().noquote() << threadTag() << "saveImageBytes write failed path=" << path
                             << "err=" << out.errorString();
        emit failed(path);
        return false;
    }

    qInfo().noquote() << threadTag() << "saveImageBytes mime=" << mimeType
                      << "suffix=" << suffix << "bytes=" << bytes.size()
                      << "path=" << path << "ok=true";
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
        qWarning().noquote() << threadTag() << "saveImage write failed path=" << path
                             << "err=" << writer.errorString();
        emit failed(path);
        return false;
    }

    qInfo().noquote() << threadTag() << "saveImage size=" << out.width() << "x" << out.height()
                      << "path=" << path << "ok=true";
    return true;
}

bool DropHandler::saveUrl(const QString &url)
{
    TextFile file;
    file.setName(FilePathProvider::nameWithPrefix("link") + ".txt");
    file.setContent(url);
    const bool ok = file.save();
    qInfo().noquote() << threadTag() << "saveUrl chars=" << url.size()
                      << "preview=" << preview(url) << "ok=" << ok;
    if (!ok)
        emit failed(url);
    return ok;
}

bool DropHandler::saveText(const QString &text)
{
    TextFile file;
    file.setName(FilePathProvider::nameWithPrefix("text") + ".txt");
    file.setContent(text);
    const bool ok = file.save();
    qInfo().noquote() << threadTag() << "saveText chars=" << text.size()
                      << "preview=" << preview(text) << "ok=" << ok;
    if (!ok)
        emit failed(text);
    return ok;
}

bool DropHandler::saveHtml(const QString &html)
{
    TextFile file;
    file.setName(FilePathProvider::nameWithPrefix("text") + ".html");
    file.setContent(html);
    const bool ok = file.save();
    qInfo().noquote() << threadTag() << "saveHtml chars=" << html.size()
                      << "preview=" << preview(html) << "ok=" << ok;
    if (!ok)
        emit failed(html);
    return ok;
}
