#ifndef TOKRIWINDOW_H
#define TOKRIWINDOW_H

#include "closebutton.h"

#include <QListView>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui {
class TokriWindow;
}
QT_END_NAMESPACE

class TokriWindow : public QMainWindow
{
    Q_OBJECT

public:
    TokriWindow(QWidget *parent = nullptr);
    ~TokriWindow();
    Ui::TokriWindow* uiHandle();
    void sleep();
    void wakeUp();

    void paintEvent(QPaintEvent *);
    void setDropping(bool status);

    void resizeEvent(QResizeEvent *e);

    void init();
    void moveNearCursor();
    void showEvent(QShowEvent *e);

public slots:
    void onShakeDetect();

private:
    Ui::TokriWindow *ui;
    bool mDropping = false;
    CloseButton *mCloseButton;
};
#endif // TOKRIWINDOW_H
