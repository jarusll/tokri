#ifndef FILEPATHPROVIDER_H
#define FILEPATHPROVIDER_H

#include "standardpaths.h"

#include <QDir>
#include <QFileInfo>
#include <QObject>
#include <QRegularExpression>
#include <QUuid>

class FilePathProvider : public QObject
{
public:

    explicit FilePathProvider(QObject *parent = nullptr);

    static QString sanitizeName(const QString &name) {
        static const QRegularExpression forbidden(R"([<>:"/\\|?*\x00-\x1F])");
        QString clean = name;
        clean.replace(forbidden, "_");
        clean = clean.trimmed();
        clean.remove(QRegularExpression(R"(^_+|_+$)"));
        if (clean.isEmpty())
            clean = "untitled";
        return clean;
    }

    static QString nameFromPath(const QString &path) {
        const QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
        return QDir(rootPath).filePath(sanitizeName(QDir(path).dirName()));
    }

    static QString nameWithPrefix(const QString &prefix = QString()) {
        const QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
        QString name = QUuid::createUuid().toString(QUuid::WithoutBraces);
        if (!prefix.isEmpty())
            name = prefix + "_" + name;
        return QDir(rootPath).filePath(name);
    }

    static QString uniquePath(const QString &desiredPath) {
        if (!QFileInfo::exists(desiredPath))
            return desiredPath;

        const QFileInfo info(desiredPath);
        const QString dir = info.absolutePath();
        const QString base = info.completeBaseName();
        const QString suffix = info.suffix();

        for (int n = 1; ; ++n) {
            QString candidate = base + "_" + QString::number(n);
            if (!suffix.isEmpty())
                candidate += "." + suffix;

            const QString path = QDir(dir).filePath(candidate);
            if (!QFileInfo::exists(path))
                return path;
        }
    }
};

#endif // FILEPATHPROVIDER_H
