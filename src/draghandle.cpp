#include "draghandle.h"

#include <QMouseEvent>
#include <QPainter>
#include <QWindow>

DragHandle::DragHandle(QWidget *parent)
    : QWidget{parent}
{
    setFixedHeight(20);
    setCursor(Qt::SizeAllCursor);
}

void DragHandle::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const int lineHeight = 6;
    const int lineWidth  = 80;
    const int radius     = lineHeight / 2;

    QRect r;
    r.setSize(QSize(lineWidth, lineHeight));
    r.moveCenter(rect().center());

    p.setPen(Qt::NoPen);

    QColor lineColor = palette().color(QPalette::Mid);
    p.setBrush(lineColor);

    p.drawRoundedRect(r, radius, radius);
}

void DragHandle::mousePressEvent(QMouseEvent *ev)
{
    if (ev->button() != Qt::LeftButton) return;
    if (window()->windowHandle())
        window()->windowHandle()->startSystemMove();
    else
        mPressOffset = ev->globalPosition().toPoint() - window()->frameGeometry().topLeft();
}

void DragHandle::mouseMoveEvent(QMouseEvent *ev)
{
    if (!(ev->buttons() & Qt::LeftButton)) return;
    if (!window()->windowHandle())
        window()->move(ev->globalPosition().toPoint() - mPressOffset);
}
