#include "copyworker.h"
#include "dropawarefilesystemmodel.h"
#include "drophandler.h"
#include "droppableswindow.h"
#include "settings.h"
#include "sortfilterproxy.h"
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
#include <QThread>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    #if defined(__linux__)
        qDebug() << "Linux";
        // define the user interface in dbus
    #endif

    DroppablesWindow w;


    DropAwareFileSystemModel *fsModel = new DropAwareFileSystemModel(&w);
    QString rootPath = Settings::get(StandardPaths::RootPath);
    QModelIndex rootIndex = fsModel->setRootPath(rootPath);

    FSSortFilterProxy *sortFilterProxy = new FSSortFilterProxy(&w);
    sortFilterProxy->setSourceModel(fsModel);

    sortFilterProxy->setDynamicSortFilter(true);
    sortFilterProxy->sort(0, Qt::DescendingOrder);

    w.uiHandle()->listView->setModel(sortFilterProxy);
    w.uiHandle()->listView->setRootIndex(sortFilterProxy->mapFromSource(rootIndex));

    TextDropHandler *dropHandler = new TextDropHandler;
    DropAwareFileSystemModel::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedText,
        dropHandler,
        &TextDropHandler::handleTextDrop
    );

    CopyWorker *worker = new CopyWorker;
    CopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedDirectory,
        worker,
        &CopyWorker::copyDirectory,
        Qt::QueuedConnection
    );
    CopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedFile,
        worker,
        &CopyWorker::copyFile,
        Qt::QueuedConnection
    );

    QThread* th = new QThread;
    CopyWorker::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    TextDropHandler::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    dropHandler->moveToThread(th);
    worker->moveToThread(th);
    th->start();

    DropAwareFileSystemModel::connect(
        w.uiHandle()->searchBar,
        &QLineEdit::textChanged,
        sortFilterProxy,
        &FSSortFilterProxy::setSearch
    );

    w.show();
    return a.exec();
}
