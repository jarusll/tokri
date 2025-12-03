#include "drophandler.h"

#include "settings.h"

#include <QRegularExpression>

DropHandler::DropHandler(QObject *parent)
    : QObject{parent}
{}

bool DropHandler::drop(const QMimeData *data)
{
    if (data->hasUrls()){
        for (const auto &url: data->urls()){
            qDebug() << url;
        }
    }

    else if (data->hasText()){
        QString rootPath = Settings::get(StandardPaths::RootPath);
        QFile textFile;
        QString text = data->text().trimmed();
        QString fileName = text.first(100);
        static const QRegularExpression invalidFileNameChars(R"([\\/:\*\?"<>\|])");
        fileName.replace(invalidFileNameChars, "_");
        textFile.setFileName(rootPath + "/" + fileName);
        if (textFile.open(QIODevice::NewOnly | QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&textFile);
            out << text;
        }
        textFile.close();
    }
    return false;
}
