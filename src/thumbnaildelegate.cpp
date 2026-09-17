#include "thumbnaildelegate.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QStyle>

namespace {

constexpr int kThumbSide = 128;
constexpr int kTopMargin = 4;
constexpr int kGap = 6;
constexpr int kBottomPadding = 4;

}

ThumbnailDelegate::ThumbnailDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

QSize ThumbnailDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    Q_UNUSED(index);

    const QFontMetrics fm(option.font);
    const int height =
        kTopMargin + kThumbSide + kGap + fm.height() + kBottomPadding;
    return QSize(kThumbSide, height);
}

void ThumbnailDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    QStyleOptionViewItem opt = option;
    initStyleOption(&opt, index);

    const QIcon icon = opt.icon;
    const QString text = opt.text;

    opt.icon = QIcon();
    opt.text.clear();
    opt.features &= ~(QStyleOptionViewItem::HasDisplay
                      | QStyleOptionViewItem::HasDecoration);

    QStyle *style = opt.widget ? opt.widget->style() : QApplication::style();
    style->drawControl(QStyle::CE_ItemViewItem, &opt, painter, opt.widget);

    const QRect cell = opt.rect;

    const QRect thumbRect(cell.x() + (cell.width() - kThumbSide) / 2,
                          cell.y() + kTopMargin,
                          kThumbSide,
                          kThumbSide);

    painter->save();
    painter->setClipRect(thumbRect);
    icon.paint(painter, thumbRect, Qt::AlignCenter, QIcon::Normal);
    painter->restore();

    const int textTop = thumbRect.bottom() + 1 + kGap;
    const QRect textRect(cell.x(), textTop, cell.width(),
                         cell.bottom() - textTop + 1);

    const QFontMetrics fm(opt.font);
    const QString elided =
        fm.elidedText(text, Qt::ElideMiddle, textRect.width());

    const QColor color =
        opt.palette.color((opt.state & QStyle::State_Selected)
                              ? QPalette::HighlightedText
                              : QPalette::Text);

    painter->save();
    painter->setPen(color);
    painter->drawText(textRect, Qt::AlignHCenter | Qt::AlignTop, elided);
    painter->restore();
}
