#include "sleekscrollbar.h"
#include "themeprovider.h"

SleekScrollBar::SleekScrollBar() {}

void SleekScrollBar::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    QStyleOptionSlider opt;
    initStyleOption(&opt);

    QRect handle = style()->subControlRect(
        QStyle::CC_ScrollBar,
        &opt,
        QStyle::SC_ScrollBarSlider,
        this
        );

    p.setPen(Qt::NoPen);
    auto pal = ThemeProvider::light();
    QColor c = pal.color(QPalette::PlaceholderText);
    c.setAlphaF(0.7);
    p.setBrush(c);
    p.drawRoundedRect(handle.adjusted(1,1,-1,-1), 4, 4);
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
