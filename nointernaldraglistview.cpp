#include "nointernaldraglistview.h"

#include <QDragEnterEvent>

NoInternalDragListView::NoInternalDragListView() {}

void NoInternalDragListView::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->source() == this)
        e->ignore();
    else {
        qDebug() << "Entered";
        emit dropping(true);
        QListView::dragEnterEvent(e);
    }
}

void NoInternalDragListView::dragMoveEvent(QDragMoveEvent *e)
{
    if (e->source() == this)
        e->ignore();
    else
        QListView::dragMoveEvent(e);
}

void NoInternalDragListView::dragLeaveEvent(QDragLeaveEvent *e)
{
    qDebug() << "Exited";
    emit dropping(false);
}
