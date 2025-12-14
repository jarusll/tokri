#ifndef CLOSEBUTTON_H
#define CLOSEBUTTON_H

#include <QAbstractButton>
#include <QObject>
#include <QPainter>

class CloseButton : public QAbstractButton
{
    Q_OBJECT
public:
    explicit CloseButton(QWidget *parent = nullptr);

protected:
    void paintEvent(QPaintEvent *) override;
};

#endif // CLOSEBUTTON_H
