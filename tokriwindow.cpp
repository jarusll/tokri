#include "tokriwindow.h"
#include "./ui_tokriwindow.h"
#include "standardpaths.h"
#include "listitemdelegate.h"
#include <QDir>
#include <QMenu>
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

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground);

    // FIXME could attach a slot to window#show for lifecycle reset of search action
    ui->searchBar->setVisible(false);

    ui->listView->setItemDelegate(new ListItemDelegate(ui->listView));

    ui->listView->setDropIndicatorShown(false);
    ui->listView->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->listView, &QWidget::customContextMenuRequested, this, [this](const QPoint &pos){
        QModelIndex index = ui->listView->indexAt(pos);
        auto selected = ui->listView->selectionModel()->selectedIndexes();
        int count = selected.size();
        QMenu menu;
        QAction *openAction = nullptr;
        QAction *openLinkAction = nullptr;
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
                // if .url.txt file, try to open as url
                // TODO add open as link option
                // if (fileInfo.fileName().endsWith(".url.txt")) {
                //     QFile file(filePath);
                //     if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                //         QByteArray content = file.readAll();
                //         QString textContent = QString::fromUtf8(content);
                //         // this can be centralized
                //         QRegularExpression urlRegex(R"(https?://[^\s]+)");
                //         QRegularExpressionMatch match = urlRegex.match(textContent);
                //         if (match.hasMatch()) {
                //             QString urlStr = match.captured(0);
                //             QDesktopServices::openUrl(QUrl(urlStr));
                //             return;
                //         }
                //     }
                // }

                QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
            } else if (chosen == revealAction) {
                QDesktopServices::openUrl(QUrl::fromLocalFile(fileInfo.absolutePath()));
            } else if (chosen == renameAction) {
                ui->listView->edit(index);
            } else if (chosen == deleteAction) {
                QFile file(filePath);
                if (file.exists()) {
                    bool success = file.remove();
                    if (!success) {
                        // FIXME handle error
                    }
                }
            } else if (chosen == copyAction) {
                QList<QUrl> urls;
                urls << QUrl::fromLocalFile(filePath);

                auto *mime = new QMimeData;
                mime->setUrls(urls);

                QGuiApplication::clipboard()->setMimeData(mime);
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
                    const auto fi = idx.data(QFileSystemModel::FileInfoRole)
                                        .value<QFileInfo>();
                    if (fi.exists())
                        urls << QUrl::fromLocalFile(fi.absoluteFilePath());
                }

                if (!urls.isEmpty()) {
                    auto *mime = new QMimeData;
                    mime->setUrls(urls);   // supports multiple files
                    QGuiApplication::clipboard()->setMimeData(mime);
                }
            } else if (chosen == deleteAction) {
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
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF r = QRectF(rect()).adjusted(2.0, 2.0, -2.0, -2.0);

    // background
    p.setPen(Qt::NoPen);
    p.setBrush(palette().color(QPalette::Window));
    p.drawRoundedRect(r, 16.0, 16.0);

    // border / drop indicator
    QColor color = palette().color(
        mDropping ? QPalette::Accent : QPalette::Shadow
        );

    QPen pen(color);
    pen.setWidthF(2.0);
    if (mDropping){
        pen.setWidthF(8.0);
    }
    pen.setJoinStyle(Qt::RoundJoin);
    pen.setCapStyle(Qt::RoundCap);

    p.setBrush(Qt::NoBrush);
    p.setPen(pen);
    p.drawRoundedRect(r, 16.0, 16.0);
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
