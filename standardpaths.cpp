#include "standardpaths.h"
#include "standardnames.h"

#include <QDir>
#include <QStandardPaths>

static QString ensure(const QString& p)
{
    QDir dir(p);
    if (!dir.exists())
        dir.mkpath(".");
    return dir.absolutePath();
}

StandardPaths::StandardPaths() {}

QString StandardPaths::getPath(Paths path)
{
    switch (path)
    {
    case TokriDir:
        return ensure(QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
            + "/" + StandardNames::get(StandardNames::Directory));
    case LinuxApplicationDir:
        return ensure(QStandardPaths::writableLocation(
            QStandardPaths::ApplicationsLocation
            ));

    case LinuxIconsDir:
        return ensure(QStandardPaths::writableLocation(
                   QStandardPaths::GenericDataLocation
                          ) + "/icons/hicolor/256x256/apps");
    default:
        return QString();
    }
}
