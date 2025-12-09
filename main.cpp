#include "copyworker.h"
#include "dropawarefilesystemmodel.h"
#include "drophandler.h"
#include "droppableswindow.h"
#include "fstolistproxy.h"
#include "settings.h"
#include "sortfilterproxy.h"
#include "ui_droppableswindow.h"

#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QApplication>
#include <QDBusConnection>
#include <QDateTime>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QMimeDatabase>
#include <QQueue>
#include <QSortFilterProxyModel>
#include <QStackedWidget>
#include <QStandardPaths>
#include <QThread>
#include <QVBoxLayout>
#include <QtDBus>

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

    FSToListProxy *listProxy = new FSToListProxy(&w);
    listProxy->setSourceModel(fsModel);
    listProxy->setRootSourceIndex(rootIndex);

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
        &DropAwareFileSystemModel::droppedFile,
        worker,
        &CopyWorker::copyFile,
        Qt::QueuedConnection
    );
    CopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedDirectory,
        worker,
        &CopyWorker::copyDirectory,
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

    // FIXME - move this to dropaware fs model
    DropAwareFileSystemModel::connect(
        w.uiHandle()->actionDelete,
        &QAction::triggered,
        w.uiHandle()->listView,
        [&w](){
            auto selectionModel = w.uiHandle()->listView->selectionModel();
            QModelIndexList indexes = selectionModel->selectedIndexes();
            for (const QModelIndex &index : std::as_const(indexes)) {
                const auto filePath = index.data(Qt::FileInfoRole).value<QFileInfo>().filePath();
                QFile(filePath).moveToTrash();
                qDebug() << filePath;
            }
        }
    );


    auto sessionBus = QDBusConnection::sessionBus();
    if (!sessionBus.isConnected()){
        qDebug() << "Failed to connect";
    }

    bool ok = sessionBus.connect(
        "oneman.jarusll.MouseShakeDetector",
        "/oneman/jarusll/MouseShakeDetector",
        "oneman.jarusll.MouseShakeDetector",
        "ShakeDetected",
        &w,
        SLOT(onShakeDetect())
    );
    qDebug() << "dbus connect ok?" << ok
             << sessionBus.lastError().name()
             << sessionBus.lastError().message();

    w.show();
    return a.exec();
}
