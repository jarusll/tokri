#ifndef FILENAMEPROVIDER_H
#define FILENAMEPROVIDER_H

#include "settings.h"

#include <QObject>
#include <QRegularExpression>

class FileNameProvider : public QObject
{
    Q_OBJECT
public:
    explicit FileNameProvider(QObject *parent = nullptr);

    static QString nameFromText(QString text){
        QString rootPath = Settings::get(StandardPaths::RootPath);
        QString fileNameBase = text.left(100);
        static const QRegularExpression invalidFileNameChars(
            R"([^A-Za-z0-9_.-])"
        );
        fileNameBase.replace(invalidFileNameChars, "_");
        fileNameBase.append(".txt");
        return QDir(rootPath).filePath(fileNameBase);
    }

signals:
};

#endif // FILENAMEPROVIDER_H
