#include "listitemdelegate.h"
#include "thumbnailprovider.h"

#include <QPainter>
#include <QFileInfo>
#include <QFileSystemModel>
#include <QApplication>

namespace {
constexpr int iconPx = 90;
constexpr int gapPx = 6;
constexpr int iconTextGap = 4;
constexpr int textBottomPadPx = 6;
constexpr int textHorzPadPx = 4;
}

ListItemDelegate::ListItemDelegate() {}

QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem &opt,
                                 const QModelIndex &) const
{
    const QFontMetrics fm(opt.font);
    const int textH = fm.ascent() + fm.descent() + textBottomPadPx;

    return QSize(
        iconPx,
        gapPx + iconPx + iconTextGap + textH
        );
}

void ListItemDelegate::paint(QPainter *p,
                             const QStyleOptionViewItem &opt,
                             const QModelIndex &idx) const
{
    QStyleOptionViewItem o(opt);
    initStyleOption(&o, idx);

    o.text.clear();
    o.icon = QIcon();

    if (opt.state & QStyle::State_Selected) {
        QColor c = opt.palette.color(QPalette::Highlight);
        p->fillRect(opt.rect, c);
    }
    else if (opt.state & QStyle::State_MouseOver) {
        QColor c = opt.palette.color(QPalette::Midlight);
        p->fillRect(opt.rect, c);
    }

    QApplication::style()->drawControl(
        QStyle::CE_ItemViewItem, &o, p);

    const QRect r = o.rect;

    // icon
    const QRect iconRect(
        r.center().x() - iconPx / 2,
        r.top() + gapPx,
        iconPx, iconPx);

    static ThumbnailProvider provider;
    provider.iconForFile(
                idx.data(QFileSystemModel::FileInfoRole).value<QFileInfo>(),
                iconRect.size())
        .paint(p, iconRect, Qt::AlignCenter);

    // text
    const QFontMetrics fm(opt.font);
    const QRect textRect(
        r.left() + textHorzPadPx,
        iconRect.bottom() + iconTextGap,
        r.width() - 2 * textHorzPadPx,
        fm.height());

    p->setPen(o.state & QStyle::State_Selected
                  ? o.palette.color(QPalette::HighlightedText)
                  : o.palette.color(QPalette::Text));

    p->drawText(
        textRect,
        Qt::AlignHCenter | Qt::AlignVCenter,
        fm.elidedText(
            idx.data(Qt::DisplayRole).toString(),
            Qt::ElideMiddle,
            textRect.width()));
}
