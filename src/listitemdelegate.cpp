#include "listitemdelegate.h"

#include <QPainter>
#include <QFileInfo>
#include <QMimeDatabase>
#include <QFileSystemModel>
#include <QApplication>

namespace {
    constexpr int iconPx = 90;
    constexpr int iconTextGap = 4;
    constexpr int gapPx  = 6;
}

ListItemDelegate::ListItemDelegate() {}

QSize ListItemDelegate::sizeHint(const QStyleOptionViewItem &opt,
                                 const QModelIndex &) const
{
    const QFontMetrics fm(opt.font);
    const int textH = fm.height();
    return QSize(
        iconPx,
        iconPx + gapPx + textH
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
        r.center().x() - iconPx/2,
        r.top() + gapPx,
        iconPx,
        iconPx
        );

    QMimeDatabase db;
    if (db.mimeTypeForFile(fi).name().startsWith("image/")) {
        QPixmap previewImage(fi.filePath());
        if (!previewImage.isNull()) {
            previewImage = previewImage.scaled(
                iconRect.size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation);
            p->drawPixmap(
                iconRect.center() - QPoint(previewImage.width() / 2, previewImage.height() / 2),
                previewImage);
        }
    } else {
        idx.data(Qt::DecorationRole).value<QIcon>()
        .paint(p, iconRect, Qt::AlignCenter);
    }

    // text
    const QRect textRect(
        r.left(),
        iconRect.bottom() + iconTextGap,
        r.width(),
        o.fontMetrics.height()
        );

    p->setPen(o.state & QStyle::State_Selected
                  ? o.palette.color(QPalette::HighlightedText)
                  : o.palette.color(QPalette::Text));

    p->drawText(
        textRect,
        Qt::AlignHCenter | Qt::AlignVCenter,
        o.fontMetrics.elidedText(
            idx.data(Qt::DisplayRole).toString(),
            Qt::ElideMiddle,
            r.width())
        );

    p->restore();
}
