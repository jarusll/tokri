#include "remoteurldrophandler.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QUuid>

RemoteUrlDropHandler::RemoteUrlDropHandler(QObject *parent)
    : QObject{parent}, networkManager(new QNetworkAccessManager(this))
{}

void RemoteUrlDropHandler::handle(QString urlStr)
{
    qDebug() << "url" << urlStr;

    QUrl url(urlStr, QUrl::StrictMode);
    if (!url.isValid()) {
        qDebug() << "invalid";
        emit droppedText(urlStr);
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

    QNetworkReply *response = networkManager->head(request);

    connect(response, &QNetworkReply::finished, this, [response, urlStr, this, url]() {
        response->deleteLater();

        if (response->error() != QNetworkReply::NoError) {
            qDebug() << "error";
            emit droppedText(urlStr);
            return;
        }

        QString contentType = response->header(QNetworkRequest::ContentTypeHeader).toString().toLower();
        QVariant cd = response->header(QNetworkRequest::ContentDispositionHeader);
        QString cdRaw = response->rawHeader("Content-Disposition");
        qDebug() << "Content type" << contentType;
        qDebug() << "Content Disposition" << cd;
        qDebug() << "Content Disposition raw" << cdRaw;

        if (contentType.startsWith("text/html")) {
            // save as url
            qDebug() << "html";
            emit droppedUrl(urlStr);
        } else if (contentType.startsWith("image")) {
            qDebug() << "image";
            QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
            QString fileName;
            // FIXME for other content disposition formats
            if (cd.isValid()) {
                fileName = cd.toString().section("filename=", 1).remove('"');
            }
            if (fileName.isEmpty()){
                fileName = QUuid::createUuid().toString(QUuid::WithoutBraces);
            }
            QString tmpFilePath = tempDir.filePath(fileName);

            QFile *tempFile = new QFile(tmpFilePath);
            bool opened = tempFile->open(QIODevice::WriteOnly);
            if (!opened){
                emit droppedText(urlStr);
            }

            QNetworkRequest greq(url);
            greq.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");
            QNetworkReply *getResponse = networkManager->get(greq);

            connect(getResponse, &QNetworkReply::readyRead, this, [getResponse, tempFile]() {
                tempFile->write(getResponse->readAll());
            });

            connect(getResponse, &QNetworkReply::finished, this, [this, getResponse, tempFile, tmpFilePath]() {
                tempFile->close();
                emit downloaded(tmpFilePath);
                delete tempFile;
                getResponse->deleteLater();
            });
        } else {
            qDebug() << "other" << contentType;
            emit droppedText(urlStr);
        }
    });
}
