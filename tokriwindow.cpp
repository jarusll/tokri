#include "tokriwindow.h"
#include "./ui_tokriwindow.h"
#include "themeprovider.h"

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
    QPen pen(palette().color(QPalette::BrightText));
    pen.setWidth(2);
    p.setBrush(Qt::NoBrush);
    p.setPen(pen);
    p.drawRoundedRect(r, 10, 10);
}

void TokriWindow::onShakeDetect()
{
    qDebug() << "Shaked";
    wakeUp();
}
