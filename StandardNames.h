#ifndef STANDARDNAMES_H
#define STANDARDNAMES_H

#include <QString>

class StandardNames {
public:
    enum Names {
        Directory,
        LockFile,
        LocalServer
    };

    static QString get(Names name){
        switch (name){
        case Directory:
            return "Wallet";
        case LockFile:
            return "oneman.jarusll.DropDogger.lock";
        case LocalServer:
            return "oneman.jarusll.DropDogger.ipc";
        default:
            return "";
        }
    }
};

#endif // STANDARDNAMES_H
