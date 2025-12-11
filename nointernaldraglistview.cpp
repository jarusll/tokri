#include "nointernaldraglistview.h"

#include <QDragEnterEvent>

NoInternalDragListView::NoInternalDragListView() {}

void NoInternalDragListView::dragEnterEvent(QDragEnterEvent *e)
{
    if (e->source() == this)
        e->ignore();
    else {
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
    emit dropping(false);
}

void NoInternalDragListView::dropEvent(QDropEvent *e)
{
    emit dropping(false);

    QListView::dropEvent(e);
}
