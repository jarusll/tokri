#ifndef SETTINGS_H
#define SETTINGS_H

#include <QDir>
#include <QObject>
#include <QStandardPaths>

class StandardNames {
public:
    enum Names {
        Directory,
    };

    static QString get(Names name){
        switch (name){
        case Directory:
            return "Wallet";
        default:
            return "";
        }
    }
};

class StandardPaths {
public:
    enum Path {
        RootPath,
    };

    static QString location(Path path){
        switch(path){
        case RootPath:
        {
            auto homePath = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
            auto home = QDir(homePath);
            // FIXME - If I dont exist, create me
            home.cd(StandardNames::get(StandardNames::Directory));
            return home.absolutePath();
        }
        default:
            return "";
        }
    }
};

class Settings : public QObject
{
public:
    explicit Settings(QObject *parent = nullptr);

    static QString get(StandardPaths::Path path){
        return StandardPaths::location(StandardPaths::RootPath);
    }
signals:
};

#endif // SETTINGS_H
