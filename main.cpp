#include "droppableswindow.h"
#include "ui_droppableswindow.h"


#include <QAbstractProxyModel>
#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QSortFilterProxyModel>
#include <QFileSystemModel>
#include <QStandardPaths>
#include <QQueue>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QAbstractItemView>

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

    FStoListProxy(QObject *parent = nullptr):rebuilds(0), QAbstractProxyModel(parent) { }

    QModelIndex mapFromSource(const QModelIndex &sourceIndex) const override {
        if (!sourceIndex.isValid())
            return QModelIndex();

        auto it = mSourceToProxyHash.find(QPersistentModelIndex(sourceIndex));
        if (it == mSourceToProxyHash.end())
            return QModelIndex();

        int proxyRow = it.value();
        int proxyColumn = sourceIndex.column();

        return createIndex(proxyRow, proxyColumn);
    }


    QModelIndex mapToSource(const QModelIndex &proxyIndex) const override {
        if (!proxyIndex.isValid()){
            return QModelIndex();
        }

        int proxyRow = proxyIndex.row();
        auto srcIndex = mSourceIndexes.at(proxyRow);
        return sourceModel()->index(
            srcIndex.row(),
            proxyIndex.column(),
            srcIndex.parent()
            );
    }

    QModelIndex parent(const QModelIndex &child) const override {
        return QModelIndex();
    }

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid()){
            return QModelIndex();
        }
        return createIndex(row, column);
    }

    int rowCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid()){
            return 0;
        }

        return mSourceIndexes.length();
    }

    int columnCount(const QModelIndex &parent = QModelIndex()) const override {
        if (parent.isValid()){
            return 0;
        }

        return 1;
    }

    QVariant data(const QModelIndex &proxyIndex, int role = Qt::DisplayRole) const override {
        if (!proxyIndex.isValid()){
            return QVariant();
        }
        QModelIndex srcIndex = mapToSource(proxyIndex);
        if (!srcIndex.isValid()){
            return QVariant();
        }
        return sourceModel()->data(srcIndex, role);
    }

    void buildIndex(const QString &directory){
        // qDebug() << "Directory " << directory;
        auto *fsm = qobject_cast<QFileSystemModel*>(sourceModel());
        if (!fsm){
            return;
        }

        beginResetModel();
        mSourceIndexes.clear();
        mSourceToProxyHash.clear();

        QModelIndex directoryIndex = fsm->index(directory, 0);
        int rowCount = fsm->rowCount(directoryIndex);
        for (int r = 0; r < rowCount; r++){
            QModelIndex sourceIndex = sourceModel()->index(r, 0, directoryIndex);
            mSourceIndexes.append(QPersistentModelIndex(sourceIndex));

            mSourceToProxyHash.insert(sourceIndex, r);
        }

        // qDebug() << "Probe " << fsm;
        // qDebug() << "RowCount " << fsm->rowCount(directoryIndex);

        endResetModel();
        rebuilds++;
        qDebug() << rebuilds;
    }
private:
    QVector<QPersistentModelIndex> mSourceIndexes;
    QHash<QPersistentModelIndex, int> mSourceToProxyHash;
    qint32 rebuilds;
};

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
    // FStoListProxy *fsToList = new FStoListProxy(&w);
    // QObject::connect(
    //     fsModel,
    //     &QFileSystemModel::directoryLoaded,
    //     fsToList,
    //     &FStoListProxy::buildIndex
    //     );
    // fsToList->setSourceModel(fsModel);
    w.uiHandle()->listView->setModel(fsModel);
    w.uiHandle()->listView->setRootIndex(rootIndex);
    w.uiHandle()->listView->setResizeMode(QListView::Adjust);

    w.uiHandle()->listView->setDragEnabled(true);
    w.uiHandle()->listView->setAcceptDrops(true);
    w.uiHandle()->listView->setDragDropMode(QAbstractItemView::DragDrop);
    w.show();
    return a.exec();
}
