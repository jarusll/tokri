#ifndef SLEEKSCROLLBAR_H
#define SLEEKSCROLLBAR_H

#include <QObject>
#include <QPainter>
#include <QScrollBar>
#include <QStyle>
#include <QStyleOptionSlider>

class SleekScrollBar : public QScrollBar
{
    Q_OBJECT
public:
    using QScrollBar::QScrollBar;

    SleekScrollBar();

protected:
    void paintEvent(QPaintEvent *) override;
    QStyleOptionSlider opt() const;
    QSize sizeHint() const override;
};

#endif // SLEEKSCROLLBAR_H
