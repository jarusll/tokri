#include "tokriwindow.h"
#include "./ui_tokriwindow.h"
#include "standardpaths.h"
#include <QDir>
#include <QMenu>
#include <QDebug>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QApplication>
#include <QClipboard>

TokriWindow::TokriWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TokriWindow)
{
    init();

    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Window);
    setAttribute(Qt::WA_TranslucentBackground);

    // FIXME could attach a slot to window#show for lifecycle reset of search action
    ui->searchBar->setVisible(false);

    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos){
        QModelIndex index = ui->listView->indexAt(pos);
        auto selected = ui->listView->selectionModel()->selectedIndexes();
        int count = selected.size();
        QMenu menu;
        QAction *openAction = nullptr;
        QAction *revealAction = nullptr;
        QAction *renameAction = nullptr;
        QAction *copyAction = nullptr;
        QAction *deleteAction = nullptr;
        QAction *selectAllAction = nullptr;

        if (count == 1) {
            openAction = menu.addAction("&Open");
            revealAction = menu.addAction("Reveal in &Explorer");
            renameAction = menu.addAction("&Rename");
            copyAction = menu.addAction("&Copy");
            deleteAction = menu.addAction("&Delete");
        } else {
            copyAction = menu.addAction("&Copy");
            deleteAction = menu.addAction("&Delete");
            selectAllAction = menu.addAction("Select &All");
        }


        QAction *chosen = menu.exec(ui->listView->viewport()->mapToGlobal(pos));
        if (!chosen) return;

        if (count == 1) {
            const auto fileInfo = selected[0].data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
            const auto filePath = selected[0].data(QFileSystemModel::FileInfoRole).value<QFileInfo>().filePath();

            if (chosen == openAction) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
                qDebug() << "Open";
            } else if (chosen == revealAction) {
                qDebug() << "Reveal in Explorer";
                QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
            } else if (chosen == renameAction) {
                qDebug() << "Rename";
                ui->listView->edit(index);
            } else if (chosen == deleteAction) {
                qDebug() << "Delete" << selected[0].row();
                QFile file(filePath);
                if (file.exists()) {
                    bool success = file.remove();
                    if (!success) {
                        // FIXME handle error
                    }
                }
            } else if (chosen == copyAction) {
                QList<QUrl> urls;
                for (const auto &idx : selected) {
                    const auto filePath = idx.data(QFileSystemModel::FileInfoRole).value<QFileInfo>().filePath();
                    QFileInfo fi(filePath);
                    if (fi.exists()) {
                        urls << QUrl::fromLocalFile(fi.absoluteFilePath());
                    }
                }
                if (!urls.isEmpty()) {
                    QMimeData *mime = new QMimeData;
                    mime->setUrls(urls);
                    QByteArray uriList;
                    for (const QUrl &u : urls) {
                        uriList += u.toString(QUrl::FullyEncoded).toUtf8();
                        uriList += '\n';
                    }
                    mime->setData("text/uri-list", uriList);
                    QApplication::clipboard()->setMimeData(mime);
                }
                qDebug() << "Copy";
            }
        } else {
            QList<QFileInfo> fileInfos;
            for (const auto &idx : selected) {
                const auto fileInfo = idx.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();
                fileInfos << fileInfo;
            }

            if (chosen == copyAction) {
                QList<QUrl> urls;
                for (const auto &idx : selected) {
                    const auto filePath = idx.data(QFileSystemModel::FileInfoRole).value<QFileInfo>().filePath();
                    QFileInfo fi(filePath);
                    if (fi.exists()) {
                        urls << QUrl::fromLocalFile(fi.absoluteFilePath());
                    }
                }
                if (!urls.isEmpty()) {
                    QMimeData *mime = new QMimeData;
                    mime->setUrls(urls);
                    QByteArray uriList;
                    for (const QUrl &u : urls) {
                        uriList += u.toString(QUrl::FullyEncoded).toUtf8();
                        uriList += '\n';
                    }
                    mime->setData("text/uri-list", uriList);
                    QApplication::clipboard()->setMimeData(mime);
                }
                qDebug() << "Copy";
            } else if (chosen == deleteAction) {
                qDebug() << "Delete" << count << "items";
                for (const auto &fileInfo : fileInfos) {
                    QFile file(fileInfo.filePath());
                    if (file.exists()) {
                        bool success = file.remove();
                        if (!success) {
                            // FIXME handle error
                        }
                    }
                }
            } else if (chosen == selectAllAction) {
                qDebug() << "Select All";
                ui->listView->selectAll();
            }
        }
    });

    connect(
        ui->listView,
        &NoInternalDragListView::dropping,
        this,
        &TokriWindow::setDropping
        );
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
    showMinimized();
}

void TokriWindow::wakeUp()
{
    if (isMinimized())
        showNormal();

    if (!isVisible())
        show();

    raise();
    activateWindow();
}

void TokriWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    const QRect r = rect().adjusted(1, 1, -1, -1);

    // fill background
    p.setPen(Qt::NoPen);
    p.setBrush(palette().color(QPalette::Window));
    p.drawRoundedRect(r, 10, 10);

    // border stroke
    auto color = palette().color(QPalette::Shadow);
    if (mDropping){
        color = palette().color(QPalette::Accent);
    }
    QPen pen(color);
    pen.setWidth(2);
    p.setBrush(Qt::NoBrush);
    p.setPen(pen);
    p.drawRoundedRect(r, 10, 10);
}

void TokriWindow::setDropping(bool status)
{
    mDropping = status;
    update();
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
}

void TokriWindow::onShakeDetect()
{
    wakeUp();
}
