#ifndef FILEPATHPROVIDER_H
#define FILEPATHPROVIDER_H

#include "settings.h"

#include <QObject>
#include <QRegularExpression>
#include <QUrl>
#include <QUuid>

class FilePathProvider : public QObject
{
public:

    explicit FilePathProvider(QObject *parent = nullptr);

    static QString nameFromText(const QString &text){
        QString rootPath = Settings::get(StandardPaths::RootPath);
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
        QString rootPath = Settings::get(StandardPaths::RootPath);
        QString fileNameBase = baseName;
        // FIXME this should be declared once
        static const QRegularExpression forbidden(R"([<>:"/\\|?*\x00-\x1F])");
        fileNameBase.replace(forbidden, "_");
        while (fileNameBase.startsWith("_")) fileNameBase.remove(0, 1);
        while (fileNameBase.endsWith("_")) fileNameBase.chop(1);
        return QDir(rootPath + "/" + fileNameBase).path();
    }

    static QString nameFromUrl(const QUrl &url){
        QString rootPath = Settings::get(StandardPaths::RootPath);
        // FIXME can do content disposition and the url itself as fallback
        QString fileName = QUuid::createUuid().toString();
        fileName.remove('{').remove('}');
        return QDir(rootPath).filePath(fileName + ".png");
    }
};

#endif // FILEPATHPROVIDER_H
