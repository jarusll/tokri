#include "textfile.h"
#include "filenameprovider.h"

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

void TextFile::save()
{
    QFile file;
    if (mName.length() > 0){
        file.setFileName(mName);
    } else {
        file.setFileName(FileNameProvider::nameFromText(mContents));
    }
    if (file.open(QIODevice::NewOnly | QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        out << mContents;
    }
    file.close();
}
