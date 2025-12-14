#include "listitemdelegate.h"

ListItemDelegate::ListItemDelegate() {}

QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const
{
    QSize s = QStyledItemDelegate::sizeHint(opt, idx);
    s.setHeight(s.height() + 16);
    return s;
}

void ListItemDelegate::paint(QPainter *p, const QStyleOptionViewItem &opt, const QModelIndex &idx) const
{
    QStyleOptionViewItem o(opt);
    o.textElideMode = Qt::ElideMiddle;
    QStyledItemDelegate::paint(p, o, idx);
}
