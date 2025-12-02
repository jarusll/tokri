#include "droppableswindow.h"
#include "./ui_droppableswindow.h"

DroppablesWindow::DroppablesWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::DroppablesWindow)
{
    ui->setupUi(this);

    setAcceptDrops(true);
}

DroppablesWindow::~DroppablesWindow()
{
    delete ui;
}

QListView* DroppablesWindow::listView()
{
    return ui->listView;
}
