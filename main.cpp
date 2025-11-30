#include "droppableswindow.h"

#include <QAbstractProxyModel>
#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>

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
        return QAbstractProxyModel::mapFromSource(sourceIndex);
    }

    QModelIndex mapToSource(const QModelIndex &proxyIndex) const {
        return QAbstractProxyModel::mapToSource(proxyIndex);
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

        return sourceModel()->rowCount();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const {
        if (parent.isValid()){
            return 1;
        }

        return 0;
    }
};

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    DroppablesWindow w;
    QFileSystemModel *fsModel = new QFileSystemModel(&w);
    FStoListProxy *fsToList = new FStoListProxy(&w);
    fsToList->setSourceModel(fsModel);
    QMimeDatabase mimeDb;
    qDebug() << mimeDb.mimeTypeForFile("/home/jarusll");
    w.show();
    return a.exec();
}
