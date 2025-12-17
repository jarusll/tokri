#ifndef STANDARDPATHS_H
#define STANDARDPATHS_H

#include <QString>

class StandardPaths
{
public:
    enum Paths {
        TokriDir,
        LinuxApplicationDir,
        LinuxIconsDir
    };

    StandardPaths();

    static QString getPath(Paths path);
};

#endif // STANDARDPATHS_H
