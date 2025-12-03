#include "drophandler.h"

#include "textfile.h"

#include <QRegularExpression>

DropHandler::DropHandler(QObject *parent)
    : QObject{parent}
{}

bool DropHandler::dropText(const QString &text)
{
    TextFile file;
    file.setContent(text);
    file.save();
    return true;
}
