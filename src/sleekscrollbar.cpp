#include "sleekscrollbar.h"
#include <QPainterPath>

SleekScrollBar::SleekScrollBar() {}

void SleekScrollBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QStyleOptionSlider opt;
    initStyleOption(&opt);

    auto pal = opt.palette;

    QColor bg = pal.color(QPalette::Base);
    QColor fg = pal.color(QPalette::PlaceholderText);

    // Clear background properly (Windows-safe)
    p.setCompositionMode(QPainter::CompositionMode_Source);
    p.fillRect(rect(), bg);
    p.setCompositionMode(QPainter::CompositionMode_SourceOver);

    QRectF bgRect = QRectF(rect()).adjusted(1, 1, -1, -1);

    // Background with square ends
    QPainterPath bgPath;
    bgPath.addRoundedRect(bgRect, 4, 4);
    bgPath.addRect(bgRect.left(), bgRect.top(), bgRect.width(), 4);
    bgPath.addRect(bgRect.left(), bgRect.bottom() - 4, bgRect.width(), 4);

    p.fillPath(bgPath, bg);

    // Slider
    QRect handle = style()->subControlRect(
        QStyle::CC_ScrollBar,
        &opt,
        QStyle::SC_ScrollBarSlider,
        this
        );

    QRectF h = QRectF(handle).adjusted(1, 2, -1, -2);
    p.setBrush(fg);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(h, 3, 3);
}

QStyleOptionSlider SleekScrollBar::opt() const {
    QStyleOptionSlider o;
    initStyleOption(&o);
    return o;
}

QSize SleekScrollBar::sizeHint() const {
    return orientation() == Qt::Vertical
               ? QSize(8, 100)
               : QSize(100, 8);
}
