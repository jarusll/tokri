#include "directorycopyworker.h"
#include "dropawarefilesystemmodel.h"
#include "drophandler.h"
#include "droppableswindow.h"
#include "settings.h"
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

    DroppablesWindow w;

    DropAwareFileSystemModel *fsModel = new DropAwareFileSystemModel(&w);
    fsModel->setReadOnly(false);

    DropHandler *dropHandler = new DropHandler(&w);
    QString rootPath = Settings::get(StandardPaths::RootPath);
    QModelIndex rootIndex = fsModel->setRootPath(rootPath);

    w.uiHandle()->listView->setModel(fsModel);
    w.uiHandle()->listView->setRootIndex(rootIndex);

    w.show();

    DropAwareFileSystemModel::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedText,
        dropHandler,
        &DropHandler::dropText
    );

    QThread* th = new QThread;
    DirectoryCopyWorker *worker = new DirectoryCopyWorker;
    worker->moveToThread(th);

    DirectoryCopyWorker::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    DirectoryCopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedDirectory,
        worker,
        &DirectoryCopyWorker::copyDirectory,
        Qt::QueuedConnection
    );
    DirectoryCopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedFile,
        worker,
        &DirectoryCopyWorker::copyFile,
        Qt::QueuedConnection
    );

    th->start();

    return a.exec();
}
