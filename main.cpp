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
#include <QLocalServer>
#include <QLocalSocket>
#include <QShortcut>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QLocalServer server;
    DroppablesWindow w;

    // Single Instance
    const QString lockPath =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                             + "/"
                             + StandardNames::get(StandardNames::LockFile);
    static QLockFile lockFile(lockPath);
    lockFile.setStaleLockTime(0);
    if (!lockFile.tryLock()) {
        QLocalSocket localSocket;
        localSocket.connectToServer(StandardNames::get(StandardNames::LocalServer));
        if (localSocket.waitForConnected(100)) {
            return 0;
        }
    } else {
        server.removeServer(StandardNames::get(StandardNames::LocalServer));
        server.listen(StandardNames::get(StandardNames::LocalServer));

        QObject::connect(&server, &QLocalServer::newConnection, [&]{
            auto sock = server.nextPendingConnection();
            if (sock) sock->close();
            w.onShakeDetect();
        });
    }


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

    auto SleepShortcut = new QShortcut(QKeySequence("Escape"), &w);
    SleepShortcut->setContext(Qt::WindowShortcut);

    QObject::connect(SleepShortcut,
                     &QShortcut::activated,
                     &w,
                     &DroppablesWindow::sleep);

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
