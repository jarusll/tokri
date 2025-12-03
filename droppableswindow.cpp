#include "droppableswindow.h"
#include "./ui_droppableswindow.h"

DroppablesWindow::DroppablesWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DroppablesWindow)
{
    ui->setupUi(this);

    // setAcceptDrops(true);
}

DroppablesWindow::~DroppablesWindow()
{
    delete ui;
}

Ui::DroppablesWindow *DroppablesWindow::uiHandle()
{
    return ui;
}

void DroppablesWindow::status(const QString &message, int timeout = 5000)
{
    ui->statusbar->showMessage(message, timeout);
}

void DroppablesWindow::statusClear()
{
    ui->statusbar->clearMessage();
}

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
