#include "tokriwindow.h"
#include "./ui_tokriwindow.h"

TokriWindow::TokriWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::TokriWindow)
{
    ui->setupUi(this);

    // This is not needed when I can detect click and shake
    // setWindowFlags(windowFlags() | Qt::WindowStaysOnTopHint);


    // FIXME could attach a slot to window#show for lifecycle reset of search action
    ui->searchBar->setVisible(false);
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

void TokriWindow::onShakeDetect()
{
    qDebug() << "Shaked";
    wakeUp();
}
