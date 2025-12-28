#include "listitemdelegate.h"

#include "themeprovider.h"

#include <QPainter>

ListItemDelegate::ListItemDelegate() {}

QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem &opt, const QModelIndex &idx) const
{
    QSize s = QStyledItemDelegate::sizeHint(opt, idx);
    s.setHeight(s.height() + 16);
    return s;
}

void ListItemDelegate::paint(QPainter *p,
                             const QStyleOptionViewItem &opt,
                             const QModelIndex &idx) const
{
    QStyleOptionViewItem o(opt);
    o.textElideMode = Qt::ElideMiddle;

    auto pal = opt.palette;

    if (o.state & QStyle::State_MouseOver) {
        QColor c = pal.color(QPalette::Midlight);
        c.setAlphaF(0.5);
        p->fillRect(o.rect, c);
    }

    o.state &= ~(QStyle::State_MouseOver | QStyle::State_HasFocus);

    QStyledItemDelegate::paint(p, o, idx);
}
