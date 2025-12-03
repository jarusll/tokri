#include "drophandler.h"

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

    if (data->hasText()){
        QFile textFile;
        QString text = data->text().trimmed();
        QString fileName = text.first(100);
        textFile.setFileName(fileName);
        if (textFile.open(QIODevice::NewOnly | QIODevice::WriteOnly | QIODevice::Text)) {
            QTextStream out(&textFile);
            out << text;
        }
        textFile.close();
    }
    return false;
}
