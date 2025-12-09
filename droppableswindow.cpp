#include "droppableswindow.h"
#include "./ui_droppableswindow.h"

DroppablesWindow::DroppablesWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DroppablesWindow)
{
    ui->setupUi(this);

    // This is not needed when I can detect click and shake
    // setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);

    ui->listView->setResizeMode(QListView::Adjust);
    ui->listView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->listView->setDragEnabled(true);
    ui->listView->setAcceptDrops(true);
    ui->listView->setDragDropMode(QAbstractItemView::DragDrop);
    ui->listView->setDefaultDropAction(Qt::CopyAction);

    // FIXME could attach a slot to window#show for lifecycle reset of search action
    ui->searchBar->setVisible(false);
}

DroppablesWindow::~DroppablesWindow()
{
    delete ui;
}

Ui::DroppablesWindow *DroppablesWindow::uiHandle()
{
    return ui;
}

void DroppablesWindow::onShakeDetect()
{
    qDebug() << "Shaked";

    if (isMinimized())
        showNormal();

    if (!isVisible())
        show();

    raise();
    activateWindow();
}
