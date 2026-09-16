#include "textfile.h"

#include <QTextStream>

TextFile::TextFile(QObject *parent)
    : QObject{parent}
{}

void TextFile::setName(QString name)
{
    mName = name;
}

void TextFile::setContent(QString content)
{
    mContents = content;
}

bool TextFile::save()
{
    if (mName.isEmpty())
        return false;

    QFile file(mName);
    if (!file.open(QIODevice::NewOnly | QIODevice::WriteOnly | QIODevice::Text))
        return false;

    QTextStream out(&file);
    out << mContents;
    return true;
}
