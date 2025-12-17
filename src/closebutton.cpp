#include "closebutton.h"

#include "themeprovider.h"

CloseButton::CloseButton(QWidget *parent)
    : QAbstractButton(parent)
{
    setFixedSize(24, 24);
    setCursor(Qt::PointingHandCursor);
    setFocusPolicy(Qt::NoFocus);
}

void CloseButton::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing, true);

    const qreal dpr = devicePixelRatioF();
    p.scale(1.0 / dpr, 1.0 / dpr);

    QRectF r(0.5, 0.5,
             width() * dpr - 1.0,
             height() * dpr - 1.0);

    const auto pal = ThemeProvider::light();
    QColor base = pal.color(QPalette::Button);
    base = base.darker(115);

    QColor bg =
        isDown()     ? base.darker(120) :
            underMouse() ? base.lighter(110) :
            base;

    p.setPen(Qt::NoPen);
    p.setBrush(bg);
    p.drawEllipse(r);

    QPen pen(Qt::white);
    pen.setWidthF(2.0 * dpr);
    pen.setCapStyle(Qt::RoundCap);
    p.setPen(pen);

    const qreal m = 7.0 * dpr;
    p.drawLine(QPointF(m, m), QPointF(r.width() - m, r.height() - m));
    p.drawLine(QPointF(r.width() - m, m), QPointF(m, r.height() - m));
}
