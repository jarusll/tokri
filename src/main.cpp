#include "copyworker.h"
#include "dropawarefilesystemmodel.h"
#include "drophandler.h"
#include "themeprovider.h"
#include "tokriwindow.h"
#include "sortfilterproxy.h"
#include "ui_tokriwindow.h"
#include "standardnames.h"
#include "standardpaths.h"

#ifdef Q_OS_WIN
#include "windowsmouseinterceptor.h"
#endif

#ifdef Q_OS_LINUX
#include "linuxmouseinterceptor.h"
#endif

#ifdef Q_OS_MACOS
#include "macosmouseinterceptor.h"
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
#include <QTimer>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setPalette(ThemeProvider::theme());


    QLocalServer server;
    TokriWindow tokriWindow;
    // Single Instance
    const QString lockFilePath =
        QStandardPaths::writableLocation(QStandardPaths::TempLocation)
                             + "/"
                             + StandardNames::get(StandardNames::LockFile);
    static QLockFile lockFile(lockFilePath);
    lockFile.setStaleLockTime(0);
    QString localServerName = StandardNames::get(StandardNames::LocalServer);

    if (!lockFile.tryLock()) {
        QLocalSocket localSocket;
        localSocket.connectToServer(localServerName);

        if (localSocket.waitForConnected(100)) {
            return 0;
        }
    } else {
        server.removeServer(localServerName);
        server.listen(localServerName);

        QObject::connect(&server, &QLocalServer::newConnection, [&]{
            auto sock = server.nextPendingConnection();
            if (sock) sock->close();
            tokriWindow.onShakeDetect();
        });
    }


    QIcon icon(":/tray.png");
    auto *tray = new QSystemTrayIcon(icon, &a);
    tray->setToolTip("Tokri - Running");
    auto *menu = new QMenu();
    menu->addAction("Show", &tokriWindow, &TokriWindow::wakeUp);
    menu->addAction("Quit", &a, &QCoreApplication::quit);
    menu->setPalette(a.palette());
    tray->setContextMenu(menu);

    QObject::connect(tray, &QSystemTrayIcon::activated,
                     [&](QSystemTrayIcon::ActivationReason r) {
                         if (r == QSystemTrayIcon::DoubleClick)
                             tokriWindow.wakeUp();
                     });

    tray->show();


    QAction *deleteAction = new QAction(&tokriWindow);
    deleteAction->setShortcut(QKeySequence::Delete);
    tokriWindow.addAction(deleteAction);

    QAction *pasteAction = new QAction(&tokriWindow);
    pasteAction->setShortcut(QKeySequence::Paste);
    tokriWindow.addAction(pasteAction);

    // View & Models
    DropAwareFileSystemModel *fsModel = new DropAwareFileSystemModel(&tokriWindow);
    QString rootPath = StandardPaths::getPath(StandardPaths::TokriDir);
    QModelIndex rootIndex = fsModel->setRootPath(rootPath);

    FSSortFilterProxy *sortFilterProxy = new FSSortFilterProxy(&tokriWindow);
    sortFilterProxy->setSourceModel(fsModel);
    sortFilterProxy->setDynamicSortFilter(true);
    sortFilterProxy->sort(0, Qt::DescendingOrder);

    tokriWindow.uiHandle()->listView->setModel(sortFilterProxy);
    tokriWindow.uiHandle()->listView->setRootIndex(sortFilterProxy->mapFromSource(rootIndex));

    TextDropHandler *dropHandler = new TextDropHandler;
    DropAwareFileSystemModel::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedText,
        dropHandler,
        &TextDropHandler::handleTextDrop
        );

    DropAwareFileSystemModel::connect(
        fsModel,
        &DropAwareFileSystemModel::droppedUrl,
        dropHandler,
        &TextDropHandler::handleUrlDrop
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


    QTimer *reloadDirectoryDebounce = new QTimer(&tokriWindow);
    reloadDirectoryDebounce->setSingleShot(true);

    bool reset = true;

    QObject::connect(worker, &CopyWorker::copySuccess,
                     reloadDirectoryDebounce,
                     [&reloadDirectoryDebounce, &reset] {
                         reloadDirectoryDebounce->setInterval(reset ? 500 : 3000);
                         reset = false;
                         reloadDirectoryDebounce->start();
                     },
                     Qt::QueuedConnection);

    QObject::connect(reloadDirectoryDebounce, &QTimer::timeout,
                     fsModel,
                     [&reset, &fsModel] {
                         reset = true;
                         const QString root = fsModel->rootPath();
                         fsModel->setRootPath(QString());
                         fsModel->setRootPath(root);
                     });


    QThread* th = new QThread;
    CopyWorker::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    TextDropHandler::connect(th, &QThread::finished, worker, &QObject::deleteLater);
    dropHandler->moveToThread(th);
    worker->moveToThread(th);
    th->start();


    // FIXME - move this to dropaware fs model
    DropAwareFileSystemModel::connect(
        deleteAction,
        &QAction::triggered,
        tokriWindow.uiHandle()->listView,
        [&tokriWindow](){
            auto selectionModel = tokriWindow.uiHandle()->listView->selectionModel();
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

    DropAwareFileSystemModel::connect(
        pasteAction,
        &QAction::triggered,
        fsModel,
        &DropAwareFileSystemModel::pasteFromClipboard
        );

    auto SleepShortcut = new QShortcut(QKeySequence("Escape"), &tokriWindow);
    SleepShortcut->setContext(Qt::WindowShortcut);

    QObject::connect(SleepShortcut,
                     &QShortcut::activated,
                     &tokriWindow,
                     &TokriWindow::sleep);


#ifdef Q_OS_LINUX
    MouseInterceptor *interceptor = new MouseInterceptor;

    QObject::connect(
        interceptor,
        &MouseInterceptor::shakeDetected,
        &tokriWindow,
        &TokriWindow::wakeUp
        );
#endif
#ifdef Q_OS_WIN
    WindowsMouseInterceptor *interceptor = new WindowsMouseInterceptor;
    interceptor->start();

    QObject::connect(
        interceptor,
        &WindowsMouseInterceptor::shakeDetected,
        &tokriWindow,
        &TokriWindow::wakeUp,
        Qt::QueuedConnection
        );
#endif

#ifdef Q_OS_MACOS
    MacOSMouseInterceptor *interceptor = new MacOSMouseInterceptor;

    QObject::connect(
        interceptor,
        &MacOSMouseInterceptor::shakeDetected,
        &tokriWindow,
        &TokriWindow::wakeUp
        );

    interceptor->start();
#endif


    tokriWindow.show();
    int ret = a.exec();

#ifdef Q_OS_WIN
    interceptor->quit();   // stop the event loop inside the thread
    interceptor->wait();   // wait for thread to finish
    delete interceptor;
#endif

    return ret;
}
