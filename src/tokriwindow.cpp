#include "tokriwindow.h"
#include "./ui_tokriwindow.h"
#include "standardpaths.h"
#include "listitemdelegate.h"
#include "closebutton.h"
#include <QDir>
#include <QMenu>
#include <QDesktopServices>
#include <QFileSystemModel>
#include <QApplication>
#include <QClipboard>
#include "sleekscrollbar.h"

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
    init();

    ui->setupUi(this);

    setWindowFlags(windowFlags()
                   | Qt::FramelessWindowHint
                   | Qt::Tool
                   | Qt::WindowStaysOnTopHint);
    setAttribute(Qt::WA_TranslucentBackground);

    mCloseButton = new CloseButton(this);
    mCloseButton->setParent(this);
    mCloseButton->raise();

    ui->listView->setStyleSheet(R"(
        QListView {
            padding-left: 8px;
        }
    )");


    auto placeClose = [this] {
        const int m = 8;
        mCloseButton->move(width() - mCloseButton->width() - m, m);
    };
    connect(
        mCloseButton,
        &QAbstractButton::clicked,
        this,
        &TokriWindow::sleep
        );

    placeClose();

    // FIXME could attach a slot to window#show for lifecycle reset of search action
    ui->searchBar->setVisible(false);


    ui->listView->setVerticalScrollBar(new SleekScrollBar(Qt::Vertical, ui->listView));

    ui->listView->setItemDelegate(new ListItemDelegate(ui->listView));

    ui->listView->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(ui->listView, &QListView::doubleClicked,
            this, [](const QModelIndex &idx){
                if (!idx.isValid())
                    return;

                const QString filePath =
                    idx.data(QFileSystemModel::FileInfoRole)
                        .value<QFileInfo>()
                        .filePath();

                QDesktopServices::openUrl(QUrl::fromLocalFile(filePath));
            });

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

                if (chosen == open && count == 1) {
                    QDesktopServices::openUrl(
                        QUrl::fromLocalFile(fileInfoAt(selected[0]).filePath()));
                    return;
                }

                if (chosen == reveal && count == 1) {
                    QDesktopServices::openUrl(
                        QUrl::fromLocalFile(fileInfoAt(selected[0]).absolutePath()));
                    return;
                }

                if (chosen == rename && count == 1) {
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
                        QFile f(fileInfoAt(idx).filePath());
                        if (f.exists())
                            f.remove();
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

#ifdef Q_OS_WIN
    HWND hWnd = reinterpret_cast<HWND>(winId());
    DWORD fgThread = GetWindowThreadProcessId(GetForegroundWindow(), nullptr);
    DWORD thisThread = GetCurrentThreadId();
    AttachThreadInput(thisThread, fgThread, TRUE);
    SetForegroundWindow(hWnd);
    SetFocus(hWnd);
    SetActiveWindow(hWnd);
    AttachThreadInput(thisThread, fgThread, FALSE);
    FlashWindow(hWnd, TRUE);
#endif
}

void TokriWindow::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const QRectF r = QRectF(rect()).adjusted(2.0, 2.0, -2.0, -2.0);

    // background
    p.setPen(Qt::NoPen);
    p.setBrush(palette().color(QPalette::Window));
    p.drawRoundedRect(r, 8.0, 8.0);

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
    p.drawRoundedRect(r, 8.0, 8.0);
}

void TokriWindow::setDropping(bool status)
{
    mDropping = status;
    update();
}

void TokriWindow::resizeEvent(QResizeEvent *e)
{
    QMainWindow::resizeEvent(e);
    const int m = 8;
    mCloseButton->move(width() - mCloseButton->width() - m, m);
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
