#ifndef DROPPABLESWINDOW_H
#define DROPPABLESWINDOW_H

#include <QListView>
#include <QMainWindow>
#include <QDragEnterEvent>
#include <QDragMoveEvent>
#include <QMimeData>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui {
class DroppablesWindow;
}
QT_END_NAMESPACE

class DroppablesWindow : public QMainWindow
{
    Q_OBJECT

public:
    DroppablesWindow(QWidget *parent = nullptr);
    ~DroppablesWindow();
    Ui::DroppablesWindow* uiHandle();

public slots:
    void onShakeDetect();

private:
    Ui::DroppablesWindow *ui;
};
#endif // DROPPABLESWINDOW_H
