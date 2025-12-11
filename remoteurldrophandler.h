#ifndef REMOTEURLDROPHANDLER_H
#define REMOTEURLDROPHANDLER_H

#include <QNetworkAccessManager>
#include <QObject>

class RemoteUrlDropHandler : public QObject
{
    Q_OBJECT
public:
    explicit RemoteUrlDropHandler(QObject *parent = nullptr);

    void handle(QString url);

private:
    QNetworkAccessManager *networkManager;
signals:
    void droppedText(QString text);
    void droppedUrl(QString url);
    void downloaded(QString path);
};

#endif // REMOTEURLDROPHANDLER_H
