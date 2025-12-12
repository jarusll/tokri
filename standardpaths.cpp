#include "standardpaths.h"
#include "StandardNames.h"

#include <QStandardPaths>

StandardPaths::StandardPaths() {}

QString StandardPaths::getPath(Paths path)
{
    switch (path)
    {
    case TokriDir:
        return QStandardPaths::writableLocation(QStandardPaths::HomeLocation)
            + "/" + StandardNames::get(StandardNames::Directory);
    default:
        return QString();
    }
}
