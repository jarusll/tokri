#ifndef DROPHANDLER_H
#define DROPHANDLER_H

#include <QImage>
#include <QMimeData>
#include <QObject>

class DropHandler : public QObject
{
    Q_OBJECT
public:
    explicit DropHandler(QObject *parent = nullptr);

public slots:
    void handleDrop(QMimeData *data);

signals:
    void changed();
    void failed(const QString &reason);

private:
    bool copyFile(const QString &filePath);
    bool copyDirectory(const QString &directory);
    bool saveImageBytes(const QByteArray &bytes, const QString &mimeType);
    bool saveImage(const QImage &image);
    bool saveUrl(const QString &url);
    bool saveText(const QString &text);
    bool saveHtml(const QString &html);
};

#endif // DROPHANDLER_H
