#include "listitemdelegate.h"

#include <QPainter>
#include <QFileInfo>
#include <QMimeDatabase>
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

    p->save();

    // base (selection / focus)
    QApplication::style()->drawPrimitive(
        QStyle::PE_PanelItemViewItem, &o, p, nullptr);

    const QRect r = o.rect;

    // hover
    if ((o.state & QStyle::State_MouseOver) &&
        !(o.state & QStyle::State_Selected)) {
        QColor c = o.palette.color(QPalette::Midlight);
        c.setAlphaF(0.5);
        p->fillRect(r, c);
    }

    const QFileInfo fi =
        idx.data(QFileSystemModel::FileInfoRole).value<QFileInfo>();

    // icon
    const QRect iconRect(
        r.center().x() - iconPx / 2,
        r.top() + gapPx,
        iconPx,
        iconPx
        );

    static QMimeDatabase db;
    if (db.mimeTypeForFile(fi).name().startsWith("image/")) {
        QPixmap pm(fi.filePath());
        if (!pm.isNull()) {
            pm = pm.scaled(
                iconRect.size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);
            p->drawPixmap(
                iconRect.center()
                    - QPoint(pm.width() / 2, pm.height() / 2),
                pm);
        }
    } else {
        idx.data(Qt::DecorationRole).value<QIcon>()
        .paint(p, iconRect, Qt::AlignCenter);
    }

    // text
    const QFontMetrics fm(o.font);
    const int textHeight = fm.ascent() + fm.descent();

    QRect textRect(
        r.left() + textHorzPadPx,
        iconRect.bottom() + iconTextGap,
        r.width() - 2 * textHorzPadPx,
        textHeight + textBottomPadPx
        );

    p->setPen(o.state & QStyle::State_Selected
                  ? o.palette.color(QPalette::HighlightedText)
                  : o.palette.color(QPalette::Text));

    p->drawText(
        textRect.adjusted(0, 0, 0, -textBottomPadPx),
        Qt::AlignHCenter | Qt::AlignBottom,
        fm.elidedText(
            idx.data(Qt::DisplayRole).toString(),
            Qt::ElideMiddle,
            textRect.width())
        );

    p->restore();
}
