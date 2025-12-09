#ifndef NOINTERNALDRAGLISTVIEW_H
#define NOINTERNALDRAGLISTVIEW_H

#include <QListView>
#include <QObject>

class NoInternalDragListView : public QListView
{
    Q_OBJECT
public:
    using QListView::QListView;

    NoInternalDragListView();

protected:
    void dragEnterEvent(QDragEnterEvent *e) override;
    void dragMoveEvent(QDragMoveEvent *e) override;
};

#endif // NOINTERNALDRAGLISTVIEW_H
