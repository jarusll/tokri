#include "drophandler.h"

#include "filepathprovider.h"
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

bool TextDropHandler::handleUrlDrop(const QString &urlStr)
{
    TextFile file;
    file.setName(FilePathProvider::nameFromUrl(urlStr) + ".url.txt");
    file.setContent(urlStr);
    file.save();
    return true;
}
