#ifndef DRAGHANDLE_H
#define DRAGHANDLE_H

#include <QWidget>

class DragHandle : public QWidget
{
    Q_OBJECT
public:
    explicit DragHandle(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
    void mousePressEvent(QMouseEvent *ev) override;
    void mouseMoveEvent(QMouseEvent *ev) override;
private:
    QPoint mPressOffset;
};

#endif // DRAGHANDLE_H
