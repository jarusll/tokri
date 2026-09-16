#include "dropawarefilesystemmodel.h"
#include "drophandler.h"
#include "loghelpers.h"
#include "tokriwindow.h"
#include "sortfilterproxy.h"
#include "ui_tokriwindow.h"
#include "standardnames.h"
#include "standardpaths.h"

#ifdef Q_OS_WIN
#include "windowsdragshakedetector.h"
#endif

#ifdef Q_OS_LINUX
#include "linuxdragshakedetector.h"
#endif

#ifdef Q_OS_MACOS
#include "macosdragshakedetector.h"
#endif

#include <QAbstractItemView>
#include <QAbstractProxyModel>
#include <QApplication>
#include <QDateTime>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QLineEdit>
#include <QMimeData>
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
#ifdef Q_OS_LINUX
    qputenv("QT_QPA_PLATFORM", "xcb");
#endif
    QApplication a(argc, argv);
    QThread::currentThread()->setObjectName("main");
    Logger::instance().log() << "Tokri starting";

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

    qRegisterMetaType<QMimeData *>("QMimeData*");

    DropHandler *dropHandler = new DropHandler;
    fsModel->setDropReceiver(dropHandler);
    Logger::instance().log() << "dropReceiver set handler="
                             << static_cast<const void *>(dropHandler);

    QObject::connect(fsModel, &DropAwareFileSystemModel::dropReceived,
                     dropHandler, &DropHandler::handleDrop,
                     Qt::QueuedConnection);

    QTimer *reloadDirectoryDebounce = new QTimer(&tokriWindow);
    reloadDirectoryDebounce->setSingleShot(true);

    bool reset = true;

    QObject::connect(dropHandler, &DropHandler::changed,
                     reloadDirectoryDebounce,
                     [&reloadDirectoryDebounce, &reset] {
                         Logger &log = Logger::instance();
                         log.push("reload");
                         reloadDirectoryDebounce->setInterval(reset ? 500 : 3000);
                         log.log() << "debounce interval="
                                   << reloadDirectoryDebounce->interval()
                                   << "reset=" << reset;
                         reset = false;
                         reloadDirectoryDebounce->start();
                         log.pop();
                     });

    QObject::connect(dropHandler, &DropHandler::failed,
                     dropHandler,
                     [](const QString &reason) {
                         Logger::instance().log() << "drop failed reason="
                                                  << reason;
                     });

    QObject::connect(reloadDirectoryDebounce, &QTimer::timeout,
                     fsModel,
                     [&reset, &fsModel] {
                         Logger &log = Logger::instance();
                         log.push("reload");
                         reset = true;
                         const QString root = fsModel->rootPath();
                         log.log() << "root=" << root;
                         fsModel->setRootPath(QString());
                         fsModel->setRootPath(root);
                         log.pop();
                     });


    QThread* th = new QThread;
    th->setObjectName("worker");
    QObject::connect(th, &QThread::finished, dropHandler, &QObject::deleteLater);
    QObject::connect(&a, &QCoreApplication::aboutToQuit, th,
                     [th] {
                         Logger::instance().log() << "aboutToQuit -> quit worker";
                         th->quit();
                     });
    QObject::connect(th, &QThread::finished, th, &QObject::deleteLater);
    dropHandler->moveToThread(th);
    Logger::instance().log() << "worker thread start handlerThread="
                             << static_cast<const void *>(dropHandler->thread());
    th->start();


    // FIXME - move this to dropaware fs model
    DropAwareFileSystemModel::connect(
        deleteAction,
        &QAction::triggered,
        tokriWindow.uiHandle()->listView,
        [&tokriWindow](){
            Logger &log = Logger::instance();
            log.push("delete");

            auto selectionModel = tokriWindow.uiHandle()->listView->selectionModel();
            QModelIndexList indexes = selectionModel->selectedIndexes();
            for (const QModelIndex &index : selectionModel->selectedIndexes()) {
                if (!index.isValid())
                    continue;

                QFileInfo fi = index.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
                const QString path = fi.filePath();

                if (fi.isDir()) {
                    log.log() << "dir path=" << path
                              << "ok=" << QDir(path).removeRecursively();
                } else {
                    log.log() << "file path=" << path
                              << "ok=" << QFile(path).moveToTrash();
                }
            }

            log.pop();
        });


    auto SleepShortcut = new QShortcut(QKeySequence("Escape"), &tokriWindow);
    SleepShortcut->setContext(Qt::WindowShortcut);

    QObject::connect(SleepShortcut,
                     &QShortcut::activated,
                     &tokriWindow,
                     &TokriWindow::sleep);


#ifdef Q_OS_LINUX
    LinuxDragShakeDetector *interceptor = new LinuxDragShakeDetector;

    QObject::connect(
        interceptor,
        &LinuxDragShakeDetector::shakeDetected,
        &tokriWindow,
        &TokriWindow::wakeUp,
        Qt::QueuedConnection
        );
#endif
#ifdef Q_OS_WIN
    WindowsDragShakeDetector *interceptor = new WindowsDragShakeDetector;

    QObject::connect(
        interceptor,
        &WindowsDragShakeDetector::shakeDetected,
        &tokriWindow,
        &TokriWindow::wakeUp,
        Qt::QueuedConnection
        );
#endif

#ifdef Q_OS_MACOS
    MacOSDragShakeDetector *interceptor = new MacOSDragShakeDetector;

    QObject::connect(
        interceptor,
        &MacOSDragShakeDetector::shakeDetected,
        &tokriWindow,
        &TokriWindow::wakeUp
        );
#endif


    tokriWindow.show();
    int ret = a.exec();

#ifdef Q_OS_WIN
    delete interceptor;
#endif

    return ret;
}
