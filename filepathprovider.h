#ifndef FILEPATHPROVIDER_H
#define FILEPATHPROVIDER_H

#include "standardpaths.h"

#include <QDir>
#include <QObject>
#include <QRegularExpression>
#include <QUrl>
#include <QUuid>

class FilePathProvider : public QObject
{
public:

    explicit FilePathProvider(QObject *parent = nullptr);

    static QString nameFromText(const QString &text){
        QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
        QString fileNameBase = text.left(80);
        // FIXME this should be declared once
        static const QRegularExpression forbidden(R"([<>:"/\\|?*\x00-\x1F])");
        fileNameBase.replace(forbidden, "_");
        while (fileNameBase.startsWith("_")) fileNameBase.remove(0, 1);
        while (fileNameBase.endsWith("_")) fileNameBase.chop(1);
        fileNameBase.append(".txt");
        return QDir(rootPath).filePath(fileNameBase);
    }

    static QString nameFromPath(const QString &path){
        QDir dir(path);
        QString baseName = dir.dirName();
        QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
        QString fileNameBase = baseName;
        // FIXME this should be declared once
        static const QRegularExpression forbidden(R"([<>:"/\\|?*\x00-\x1F])");
        fileNameBase.replace(forbidden, "_");
        while (fileNameBase.startsWith("_")) fileNameBase.remove(0, 1);
        while (fileNameBase.endsWith("_")) fileNameBase.chop(1);
        return QDir(rootPath + "/" + fileNameBase).path();
    }

    static QString nameFromUrl(const QUrl &url){
        QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
        // FIXME can do content disposition and the url itself as fallback
        static const QRegularExpression forbidden(R"([<>:"/\\|?*\x00-\x1F])");
        QString urlStr = url.toString();
        urlStr.replace(forbidden, "_");
        return QDir(rootPath).filePath(urlStr + ".png");
    }

    static QString nameWithPrefix(QString prefix = QString()){
        QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
        QString uuidStr = QUuid::createUuid().toString();
        uuidStr.replace("{", "");
        uuidStr.replace("}", "");
        if (!prefix.isEmpty()){
            uuidStr = prefix + "_" + uuidStr;
        }
        return QDir(rootPath).filePath(uuidStr);
    }
};

#endif // FILEPATHPROVIDER_H
