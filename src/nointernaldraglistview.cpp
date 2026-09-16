#include "nointernaldraglistview.h"

#include "loghelpers.h"

#include <QDragEnterEvent>

NoInternalDragListView::NoInternalDragListView() {}

void NoInternalDragListView::dragEnterEvent(QDragEnterEvent *e)
{
    const bool isSelf = e->source() == this;
    qInfo().noquote() << threadTag() << "dragEnter source==this=" << isSelf
                      << "formats=" << e->mimeData()->formats();
    if (isSelf)
        e->ignore();
    else {
        emit dropping(true);
        QListView::dragEnterEvent(e);
    }
    qInfo().noquote() << threadTag() << "dragEnter accepted=" << e->isAccepted();
}

void NoInternalDragListView::dragMoveEvent(QDragMoveEvent *e)
{
    const bool isSelf = e->source() == this;
    if (isSelf)
        e->ignore();
    else
        QListView::dragMoveEvent(e);
    qInfo().noquote() << threadTag() << "dragMove source==this=" << isSelf
                      << "accepted=" << e->isAccepted();
}

void NoInternalDragListView::dragLeaveEvent(QDragLeaveEvent *e)
{
    qInfo().noquote() << threadTag() << "dragLeave";
    emit dropping(false);

    QListView::dragLeaveEvent(e);
}

void NoInternalDragListView::dropEvent(QDropEvent *e)
{
    qInfo().noquote() << threadTag() << "drop ENTER source==this=" << (e->source() == this)
                      << "dropAction=" << e->dropAction()
                      << "possibleActions=" << e->possibleActions()
                      << "formats=" << e->mimeData()->formats();
    emit dropping(false);

    QListView::dropEvent(e);

    qInfo().noquote() << threadTag() << "drop accepted=" << e->isAccepted()
                      << "finalDropAction=" << e->dropAction();
}

void NoInternalDragListView::paintEvent(QPaintEvent *e) {
    QListView::paintEvent(e);

    auto *m = model();
    if (!m || m->rowCount(rootIndex()) > 0)
        return;

    QPainter p(viewport());

    QPixmap pm(":/background.png");
    pm = pm.scaled(150, 150,
                   Qt::KeepAspectRatio,
                   Qt::SmoothTransformation);

    const QString text =
        "Drop files, folders, text, links, or images here.";

    QFontMetrics fm(font());
    int textHeight = fm.height();
    int spacing = 8;

    int totalHeight = pm.height() + spacing + textHeight;

    int startY = (height() - totalHeight) / 3;

    QPoint iconPos(width()/2 - pm.width()/2,
                   startY);
    p.drawPixmap(iconPos, pm);

    QRect textRect(0,
                   startY + pm.height() + spacing,
                   width(),
                   textHeight);

    p.setPen(palette().color(QPalette::Text));
    p.drawText(textRect, Qt::AlignHCenter, text);
}
