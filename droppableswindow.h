#ifndef DROPPABLESWINDOW_H
#define DROPPABLESWINDOW_H

#include <QMainWindow>

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

private:
    Ui::DroppablesWindow *ui;
};
#endif // DROPPABLESWINDOW_H
