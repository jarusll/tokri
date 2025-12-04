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
    w.setWindowFlags(w.windowFlags() | Qt::WindowStaysOnTopHint);

    DropAwareFileSystemModel *fsModel = new DropAwareFileSystemModel(&w);
    DropHandler *dropHandler = new DropHandler(&w);
    DropAwareFileSystemModel::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedText,
        dropHandler,
        &DropHandler::dropText
    );

    fsModel->setReadOnly(false);
    QString rootPath = Settings::get(StandardPaths::RootPath);
    QModelIndex rootIndex = fsModel->setRootPath(rootPath);

    w.uiHandle()->listView->setModel(fsModel);
    w.uiHandle()->listView->setRootIndex(rootIndex);
    w.uiHandle()->listView->setResizeMode(QListView::Adjust);
    w.uiHandle()->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    w.uiHandle()->listView->setDragEnabled(true);
    w.uiHandle()->listView->setAcceptDrops(true);
    w.uiHandle()->listView->setDragDropMode(QAbstractItemView::DragDrop);
    w.uiHandle()->listView->setDefaultDropAction(Qt::CopyAction);

    w.show();

    QThread* th = new QThread;
    DirectoryCopyWorker *worker = new DirectoryCopyWorker;
    worker->moveToThread(th);

    DirectoryCopyWorker::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    DirectoryCopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedDirectory,
        worker,
        &DirectoryCopyWorker::copy,
        Qt::QueuedConnection
    );

    // DirectoryCopyWorker::connect(
    //     worker,
    //     &DirectoryCopyWorker::copying,
    //     &w,
    //     [&w](const QString &path){
    //         w.status("Copying " + path, 1000);
    //     }
    // );
    // DirectoryCopyWorker::connect(
    //     worker,
    //     &DirectoryCopyWorker::copied,
    //     &w,
    //     &DroppablesWindow::statusClear
    // );

    th->start();

    return a.exec();
}
