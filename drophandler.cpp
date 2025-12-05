#include "drophandler.h"

#include "textfile.h"

#include <QRegularExpression>

TextDropHandler::TextDropHandler(QObject *parent)
    : QObject{parent}
{}

bool TextDropHandler::handleTextDrop(const QString &text)
{
    TextFile file;
    file.setContent(text);
    file.save();
    return true;
}
