#include "remoteurldrophandler.h"

#include "filepathprovider.h"

#include <QNetworkRequest>
#include <QNetworkReply>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QImage>
#include <QUuid>

RemoteUrlDropHandler::RemoteUrlDropHandler(QObject *parent)
    : QObject{parent}, networkManager(new QNetworkAccessManager(this))
{}

void RemoteUrlDropHandler::handle(QString urlStr)
{
    QUrl url(urlStr, QUrl::StrictMode);
    if (!url.isValid()) {
        emit droppedText(urlStr);
        return;
    }

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::UserAgentHeader, "Mozilla/5.0");

    QNetworkReply *response = networkManager->head(request);

    connect(response, &QNetworkReply::finished, this, [response, urlStr, this, url]() {
        response->deleteLater();

        if (response->error() != QNetworkReply::NoError) {
            emit droppedText(urlStr);
            return;
        }

        QString contentType = response->header(QNetworkRequest::ContentTypeHeader).toString().toLower();

        if (contentType.startsWith("image")) {
            QDir tempDir(QStandardPaths::writableLocation(QStandardPaths::TempLocation));
            QString fileName;
            QString tmpFilePath = FilePathProvider::nameFromUrl(urlStr);

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
            emit droppedText(urlStr);
        }
    });
}
