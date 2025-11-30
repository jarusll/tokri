#include "droppableswindow.h"

#include <QAbstractProxyModel>
#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QStandardPaths>
#include <QQueue>

enum class FileType {
    Audio,
    Image,
    Video,
    // FIXME: No explicit mime types available for this
    Document,
    Text,
    Other
};

enum class EntryType {
    Directory,
    File
};

// All prefix types in Qt
// video
// message
// x-content
// chemical
// multipart
// model
// application
// font
// text
// image
// x-epoc
// inode
// audio
// FIXME - what about directory?
FileType qtMimeToFileType(const QMimeType &mt){
    QString mimeString = mt.name();
    QList<QString> mimeSplit = mimeString.split("/");
    QString mimePrefix = mimeSplit.at(0);
    if (mimePrefix == "audio"){
        return FileType::Audio;
    }
    if (mimePrefix == "video"){
        return FileType::Video;
    }
    if (mimePrefix == "image"){
        return FileType::Image;
    }
    if (mimePrefix == "text"){
        return FileType::Text;
    }
    return FileType::Other;
}

struct Entry {
    EntryType entryType;
    FileType fileType;
    QString path; // QFileInfo#filePath
    QString name; // QFileInfo#fileName
    QDateTime created; // QFileInfo#birthTime
};

class FStoListProxy: public QAbstractProxyModel {

public:

    FStoListProxy(QObject *parent = nullptr):QAbstractProxyModel(parent) { }

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const {
        return QModelIndex();
        // return QAbstractProxyModel::mapFromSource(sourceIndex);
    }

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const {
        return QModelIndex();
        // return QAbstractProxyModel::mapToSource(proxyIndex);
    }

    QModelIndex parent(const QModelIndex &child) const {
        return QModelIndex();
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const {
        return QModelIndex();
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const {
        if (parent.isValid()){
            return 0;
        }

        return mVectorIndexes.length();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const {
        if (parent.isValid()){
            return 1;
        }

        return 0;
    }

    void buildIndex(const QString &directory){
        qDebug() << "Directory " << directory;
        auto *fsm = qobject_cast<QFileSystemModel*>(sourceModel());
        if (!fsm){
            return;
        }

        beginResetModel();
        mVectorIndexes.clear();

        QModelIndex directoryIndex = fsm->index(directory, 0);
        int rowCount = fsm->rowCount(directoryIndex);
        for (int r = 0; r < rowCount; r++){
            QModelIndex index = sourceModel()->index(r, 0, directoryIndex);
            mVectorIndexes.append(QPersistentModelIndex(index));
        }

        qDebug() << "Probe " << fsm;
        qDebug() << "RowCount " << fsm->rowCount(directoryIndex);

        endResetModel();
    }
private:
    QVector<QPersistentModelIndex> mVectorIndexes;
};

class Names {
public:
    enum Name {
        Directory,
    };

    static QString get(Name name){
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
            home.cd(Names::get(Names::Directory));
            return home.absolutePath();
        }
        default:
            return "";
        }
    }
};


class Settings {
public:
    static QString get(StandardPaths::Path path){
        return StandardPaths::location(StandardPaths::RootPath);
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DroppablesWindow w;
    QFileSystemModel *fsModel = new QFileSystemModel(&w);
    QString rootPath = Settings::get(StandardPaths::RootPath);
    QModelIndex rootIndex = fsModel->setRootPath(rootPath);
    FStoListProxy *fsToList = new FStoListProxy(&w);
    QObject::connect(
        fsModel,
        &QFileSystemModel::directoryLoaded,
        fsToList,
        &FStoListProxy::buildIndex
    );
    fsToList->setSourceModel(fsModel);
    w.show();
    return a.exec();
}
