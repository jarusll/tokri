#include "tokriwindow.h"
#include "./ui_tokriwindow.h"
#include "standardpaths.h"
#include <QDir>

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
