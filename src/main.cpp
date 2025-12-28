#include "copyworker.h"
#include "dropawarefilesystemmodel.h"
#include "drophandler.h"
#include "themeprovider.h"
#include "tokriwindow.h"
#include "sortfilterproxy.h"
#include "ui_tokriwindow.h"
#include "remoteurldrophandler.h"
#include "standardnames.h"
#include "standardpaths.h"

#ifdef Q_OS_WIN
#include "windowsmouseinterceptor.h"
#endif

#ifdef Q_OS_LINUX
#include "linuxmouseinterceptor.h"
#endif

#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QApplication>
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
#include <QLocalServer>
#include <QLocalSocket>
#include <QShortcut>
#include <QLockFile>
#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    a.setPalette(ThemeProvider::theme());

    QIcon icon(":/tray.png");

    QLocalServer server;
    TokriWindow w;

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

    // Actions
    // QAction *searchAction = new QAction(&w);
    // searchAction->setShortcut(QKeySequence::Find);
    // searchAction->setCheckable(true);
    // searchAction->setChecked(false);
    // w.addAction(searchAction);
    // TokriWindow::connect(searchAction, &QAction::toggled,
    //         [&w](bool on){
    //             w.uiHandle()->searchBar->setVisible(on);
    //         });

    auto *tray = new QSystemTrayIcon(icon, &a);
    tray->setToolTip("Tokri - Running");
    auto *menu = new QMenu();
    menu->addAction("Show", &w, &TokriWindow::wakeUp);
    menu->addAction("Quit", &a, &QCoreApplication::quit);
    menu->setPalette(ThemeProvider::theme());
    tray->setContextMenu(menu);
    tray->show();

    QAction *deleteAction = new QAction(&w);
    deleteAction->setShortcut(QKeySequence::Delete);
    w.addAction(deleteAction);

    // View & Models

    DropAwareFileSystemModel *fsModel = new DropAwareFileSystemModel(&w);
    QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
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

    RemoteUrlDropHandler *urlHandler = new RemoteUrlDropHandler(&w);
    DropAwareFileSystemModel::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedUrl,
        dropHandler,
        &TextDropHandler::handleUrlDrop
        );

    RemoteUrlDropHandler::connect(
        urlHandler,
        &RemoteUrlDropHandler::droppedText,
        dropHandler,
        &TextDropHandler::handleTextDrop
        );
    RemoteUrlDropHandler::connect(
        urlHandler,
        &RemoteUrlDropHandler::droppedUrl,
        dropHandler,
        &TextDropHandler::handleTextDrop
        );

    CopyWorker *worker = new CopyWorker;
    RemoteUrlDropHandler::connect(
        urlHandler,
        &RemoteUrlDropHandler::downloaded,
        worker,
        &CopyWorker::copyFile,
        Qt::QueuedConnection
        );

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
    CopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedImage,
        worker,
        &CopyWorker::saveImage,
        Qt::QueuedConnection
        );
    CopyWorker::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedImageBytes,
        worker,
        &CopyWorker::saveImageBytes,
        Qt::QueuedConnection
        );

    QThread* th = new QThread;
    CopyWorker::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    TextDropHandler::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    dropHandler->moveToThread(th);
    worker->moveToThread(th);
    th->start();

    // DropAwareFileSystemModel::connect(
    //     w.uiHandle()->searchBar,
    //     &QLineEdit::textChanged,
    //     sortFilterProxy,
    //     &FSSortFilterProxy::setSearch
    //     );

    // FIXME - move this to dropaware fs model
    DropAwareFileSystemModel::connect(
        deleteAction,
        &QAction::triggered,
        w.uiHandle()->listView,
        [&w](){
            auto selectionModel = w.uiHandle()->listView->selectionModel();
            QModelIndexList indexes = selectionModel->selectedIndexes();
            for (const QModelIndex &index : selectionModel->selectedIndexes()) {
                if (!index.isValid())
                    continue;

                QFileInfo fi = index.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
                const QString path = fi.filePath();

                if (fi.isDir()) {
                    QDir(path).removeRecursively();
                } else {
                    QFile(path).moveToTrash();
                }
            }
        });

    auto SleepShortcut = new QShortcut(QKeySequence("Escape"), &w);
    SleepShortcut->setContext(Qt::WindowShortcut);

    QObject::connect(SleepShortcut,
                     &QShortcut::activated,
                     &w,
                     &TokriWindow::sleep);


#ifdef Q_OS_LINUX
    MouseInterceptor *interceptor = new MouseInterceptor;

    QObject::connect(
        interceptor,
        &MouseInterceptor::shakeDetected,
        &w,
        &TokriWindow::wakeUp
        );
#endif
#ifdef Q_OS_WIN
    WindowsMouseInterceptor *interceptor = new WindowsMouseInterceptor;
    interceptor->start();

    QObject::connect(
        interceptor,
        &WindowsMouseInterceptor::shakeDetected,
        &w,
        &TokriWindow::wakeUp,
        Qt::QueuedConnection
        );
#endif

    w.show();
    int ret = a.exec();

#ifdef Q_OS_WIN
    interceptor->quit();   // stop the event loop inside the thread
    interceptor->wait();   // wait for thread to finish
    delete interceptor;
#endif

    return ret;
}
