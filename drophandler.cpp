#include "drophandler.h"

#include "textfile.h"

#include <QRegularExpression>

DropHandler::DropHandler(QObject *parent)
    : QObject{parent}
{}

bool DropHandler::drop(const QMimeData *data)
{
    if (data->hasUrls()){
        for (const auto &url: data->urls()){
            qDebug() << url.fileName() << url.isLocalFile();
        }
    }

    else if (data->hasText()){
        TextFile file;
        file.setContent(data->text().trimmed());
        file.save();
    }
    return false;
}
