#include "thumbnaildelegate.h"

#include "thumbnaillayout.h"

#include <QApplication>
#include <QFontMetrics>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QStyle>
#include <QWidget>

namespace {

void paintTile(QPainter *painter, const QRect &cell, const QPixmap &content,
               const QPalette &palette)
{
    using namespace ThumbnailLayout;

    const QRect container(cell.x() + (cell.width() - Container) / 2,
                          cell.y(), Container, Container);
    const QSize maxContent{ContentSide, ContentSide};

    QSize contentSize = content.deviceIndependentSize().toSize();
    const bool scaled = contentSize.width() > maxContent.width()
                        || contentSize.height() > maxContent.height();
    if (scaled)
        contentSize.scale(maxContent, Qt::KeepAspectRatio);

    QRect contentRect(QPoint(0, 0), contentSize);
    contentRect.moveCenter(container.center());

    const int inset = Padding + BorderWidth;
    const QRect frameRect = contentRect.adjusted(-inset, -inset, inset, inset);

    const qreal radius = BorderRadius;
    QPainterPath framePath;
    framePath.addRoundedRect(QRectF(frameRect), radius, radius);

    painter->save();
    painter->setRenderHint(QPainter::Antialiasing, true);

    painter->setPen(Qt::NoPen);
    painter->setBrush(palette.color(QPalette::Base));
    painter->drawPath(framePath);

    QPen borderPen(Qt::white);
    borderPen.setWidth(BorderWidth);
    painter->setPen(borderPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(framePath);

    painter->restore();

    if (scaled)
        painter->drawPixmap(contentRect, content);
    else
        painter->drawPixmap(contentRect.topLeft(), content);
}

}

ThumbnailDelegate::ThumbnailDelegate(QObject *parent)
    : QStyledItemDelegate(parent)
{}

QSize ThumbnailDelegate::sizeHint(const QStyleOptionViewItem &option,
                                  const QModelIndex &index) const
{
    Q_UNUSED(index);

    using namespace ThumbnailLayout;

    const QFontMetrics fm(option.font);
    return QSize(TileWidth, TileHeight(fm.height()));
}

void ThumbnailDelegate::paint(QPainter *painter,
                              const QStyleOptionViewItem &option,
                              const QModelIndex &index) const
{
    using namespace ThumbnailLayout;

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

    const qreal dpr = opt.widget ? opt.widget->devicePixelRatioF()
                                 : qApp->devicePixelRatio();

    QPixmap content =
        index.data(ThumbnailPixmapRole).value<QPixmap>();
    if (content.isNull() && !icon.isNull())
        content = icon.pixmap(QSize(ContentSide, ContentSide), dpr);

    if (!content.isNull())
        paintTile(painter, cell, content, opt.palette);

    const int textTop =
        cell.y() + Margin + Container + 1 + CaptionMargin;
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
