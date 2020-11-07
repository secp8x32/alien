#include <QNetworkRequest>
#include <QNetworkReply>

#include "HttpClient.h"

HttpClient::HttpClient(QObject* parent /*= nullptr*/)
    : QObject(parent)
{
    connect(&_networkManager, &QNetworkAccessManager::finished, this, &HttpClient::finished);

    //TODO: QNetworkAccessManager::authenticationRequired
}

void HttpClient::get(QUrl const& url, int handler)
{
    auto reply = _networkManager.get(QNetworkRequest(url));
    _handlerByReply.insert_or_assign(reply, handler);
}

void HttpClient::post(QUrl const & url, int handler, QByteArray const & data)
{
    auto reply = _networkManager.post(QNetworkRequest(url), data);
    _handlerByReply.insert_or_assign(reply, handler);
}

void HttpClient::finished(QNetworkReply * reply)
{
    if (QNetworkReply::NetworkError::NoError != reply->error()) {
        Q_EMIT error("Could not read data from server.");
        return;
    }
    auto data = reply->readAll();

    auto handler = _handlerByReply.at(reply);
    Q_EMIT dataReceived(handler, data);

    reply->deleteLater();
}