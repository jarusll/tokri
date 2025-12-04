#include "droppableswindow.h"
#include "./ui_droppableswindow.h"

DroppablesWindow::DroppablesWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DroppablesWindow)
{
    ui->setupUi(this);

    setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setDragEnabled(true);
    ui->listView->setAcceptDrops(true);
    ui->listView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->listView->setDefaultDropAction(Qt::CopyAction);
}

DroppablesWindow::~DroppablesWindow()
{
    delete ui;
}

Ui::DroppablesWindow *DroppablesWindow::uiHandle()
{
    return ui;
}

// void DroppablesWindow::status(const QString &message, int timeout = 5000)
// {
//     ui->statusbar->showMessage(message, timeout);
// }

// void DroppablesWindow::statusClear()
// {
//     ui->statusbar->clearMessage();
// }

// void DroppablesWindow::paintEvent(QPaintEvent *)
// {
//     QPainter p(this);
//     p.setRenderHint(QPainter::Antialiasing);
//     p.setBrush(QColor(30,30,30));
//     p.setPen(Qt::NoPen);
//     p.drawRoundedRect(rect(), 12, 12);   // radius
// }

// void DroppablesWindow::dragEnterEvent(QDragEnterEvent *event)
// {
//     if (event->mimeData()->hasText() || event->mimeData()->hasUrls()){
//         event->acceptProposedAction();
//     }
// }

// void DroppablesWindow::dragMoveEvent(QDragMoveEvent *event)
// {
//     if (event->mimeData()->hasText() || event->mimeData()->hasUrls()){
//         event->acceptProposedAction();
//         qDebug() << event->mimeData()->text() << event->mimeData()->urls();
//     }
// }
