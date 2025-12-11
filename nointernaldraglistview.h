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
    void dragLeaveEvent(QDragLeaveEvent *e) override;
    void dropEvent(QDropEvent *e) override;
signals:
    void dropping(bool status);
};

#endif // NOINTERNALDRAGLISTVIEW_H
