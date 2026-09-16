#include "textfile.h"

#include "loghelpers.h"

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
    if (mName.isEmpty()) {
        qWarning().noquote() << threadTag() << "TextFile::save empty name -> false";
        return false;
    }

    QFile file(mName);
    if (!file.open(QIODevice::NewOnly | QIODevice::WriteOnly | QIODevice::Text)) {
        qWarning().noquote() << threadTag() << "TextFile::save open failed name=" << mName
                             << "err=" << file.errorString();
        return false;
    }

    QTextStream out(&file);
    out << mContents;

    if (out.status() != QTextStream::Ok) {
        qWarning().noquote() << threadTag() << "TextFile::save write failed name=" << mName
                             << "status=" << out.status();
        return false;
    }

    qInfo().noquote() << threadTag() << "TextFile::save name=" << mName
                      << "chars=" << mContents.size() << "ok=true";
    return true;
}
