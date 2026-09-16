#include "tokriwindow.h"
#include "./ui_tokriwindow.h"
#include "standardpaths.h"
#include <QDir>
#include <QMenu>
#include <QCloseEvent>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QApplication>
#include <QClipboard>

#ifdef Q_OS_WIN
#include <windows.h>
#endif

#ifdef Q_OS_MAC
#include "MacWindowLevel.h"
#endif

TokriWindow::TokriWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TokriWindow)
{
    ui->setupUi(this);

    init();
    setWindowFlags(windowFlags()
                   | Qt::WindowStaysOnTopHint);

    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->listView, &QListView::doubleClicked,
            this, [this](const QModelIndex &idx){
                if (!idx.isValid())
                    return;

                const QString filePath =
                    idx.data(QFileSystemModel::FileInfoRole)
                        .value<QFileInfo>()
                        .filePath();
                openItem(filePath);
    });

    ui->listView->setViewMode(QListView::IconMode);
    ui->listView->setGridSize({100, 130});
    ui->listView->setFlow(QListView::LeftToRight);
    ui->listView->setWrapping(true);
    ui->listView->setUniformItemSizes(true);
    ui->listView->setSpacing(8);
    ui->listView->setMouseTracking(true);
    ui->listView->setFocusPolicy(Qt::NoFocus);
    ui->listView->setDropIndicatorShown(false);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);

    connect(ui->listView, &QWidget::customContextMenuRequested, this,
            [this](const QPoint &pos) {
                auto *view = ui->listView;
                auto *sel  = view->selectionModel();
                const auto selected = sel->selectedIndexes();
                const int count = selected.size();

                QMenu menu;
                menu.setPalette(this->palette());

                QAction *open = nullptr, *reveal = nullptr, *rename = nullptr;
                QAction *copy = nullptr, *del = nullptr, *selectAll = nullptr;

                if (count == 1) {
                    open   = menu.addAction("&Open");
                    reveal= menu.addAction("Reveal in &Explorer");
                    rename= menu.addAction("&Rename");
                }
                if (count > 0) {
                    copy = menu.addAction("&Copy");
                    del  = menu.addAction("&Delete");
                }
                selectAll = menu.addAction("Select &All");

                QAction *chosen = menu.exec(view->viewport()->mapToGlobal(pos));
                if (!chosen) return;

                auto fileInfoAt = [](const QModelIndex &idx) {
                    return idx.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
                };

                if (chosen == selectAll) {
                    view->selectAll();
                    return;
                }

                if (count == 1 && chosen == open) {
                    QString filePath = fileInfoAt(selected[0]).filePath();
                    openItem(filePath);
                    return;
                }

                if (count == 1 && chosen == reveal) {
                    QDesktopServices::openUrl(
                        QUrl::fromLocalFile(fileInfoAt(selected[0]).absolutePath()));
                    return;
                }

                if (count == 1 && chosen == rename) {
                    view->edit(selected[0]);
                    return;
                }

                if (chosen == copy) {
                    QList<QUrl> urls;
                    for (const auto &idx : selected) {
                        const auto fi = fileInfoAt(idx);
                        if (fi.exists())
                            urls << QUrl::fromLocalFile(fi.absoluteFilePath());
                    }
                    if (!urls.isEmpty()) {
                        auto *mime = new QMimeData;
                        mime->setUrls(urls);
                        QGuiApplication::clipboard()->setMimeData(mime);
                    }
                    return;
                }

                if (chosen == del) {
                    for (const auto &idx : selected) {
                        QFileInfo fi(fileInfoAt(idx).filePath());
                        if (!fi.exists())
                            return;

                        if (fi.isDir())
                            QDir(fi.absoluteFilePath()).removeRecursively();
                        else
                            QFile::remove(fi.absoluteFilePath());                    }
                }
            });

}

TokriWindow::~TokriWindow()
{
    delete ui;
}

Ui::TokriWindow *TokriWindow::uiHandle()
{
    return ui;
}

void TokriWindow::sleep()
{
    hide();
}

void TokriWindow::wakeUp()
{
    const bool minimized = isMinimized();
    const bool hidden = !isVisible();

    if (minimized || hidden) {
        moveNearCursor();
    }

    if (minimized) {
        showNormal();
    } else if (hidden) {
        show();
    }

    raise();
    activateWindow();
}

void TokriWindow::init()
{
    QString tokriDir = StandardPaths::getPath(StandardPaths::TokriDir);
    QDir dir(tokriDir);
    if (!dir.exists()){
        bool success = dir.mkpath(tokriDir);
        if (!success){
            // FIXME handle error
        }
    }
#ifdef Q_OS_MAC
    MacWindowLevel::hideFromDock();
#endif
}

void TokriWindow::moveNearCursor()
{
    const QPoint cursor = QCursor::pos();
    const QSize  winSize = size();
    QPoint p(cursor.x() + 20, cursor.y() + 20);

    const QRect screen = QGuiApplication::screenAt(cursor)->availableGeometry();

    if (p.x() + winSize.width() > screen.right())
        p.setX(screen.right() - winSize.width());
    if (p.y() + winSize.height() > screen.bottom())
        p.setY(screen.bottom() - winSize.height());
    if (p.x() < screen.left())
        p.setX(screen.left());
    if (p.y() < screen.top())
        p.setY(screen.top());

    move(p);
}

void TokriWindow::onShakeDetect()
{
    wakeUp();
}


void TokriWindow::showEvent(QShowEvent *e)
{
    QWidget::showEvent(e);
#ifdef Q_OS_MAC
    MacWindowLevel::makeAlwaysOnTop(windowHandle());
#endif
}

void TokriWindow::openItem(QString filePath) {
    QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
}

void TokriWindow::closeEvent(QCloseEvent *e)
{
    e->ignore();
    sleep();
}

