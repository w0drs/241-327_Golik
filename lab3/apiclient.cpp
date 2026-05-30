#include "apiclient.h"
#include "animeadapter.h"
#include <QJsonDocument>
#include <QSslConfiguration>
#include <QSslSocket>
#include <QDebug>
#include <QNetworkRequest>

ApiClient* ApiClient::instance = nullptr;
QMutex ApiClient::mutex;

ApiClient::ApiClient(QObject *parent)
    : QObject(parent)
    , baseUrl("https://localhost/")
{
    networkManager = new QNetworkAccessManager(this);

    if (!networkManager) {
        qCritical() << "Failed to create QNetworkAccessManager";
        return;
    }

    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration::setDefaultConfiguration(sslConfig);

    connect(networkManager, &QNetworkAccessManager::sslErrors,
            this, [](QNetworkReply* reply, const QList<QSslError> &errors){
                reply->ignoreSslErrors();
            });

    connect(networkManager, &QNetworkAccessManager::finished,
            this, &ApiClient::onReplyFinished);

    qDebug() << "ApiClient initialized with URL:" << baseUrl;
}

ApiClient::~ApiClient() {
    if (networkManager) {
        delete networkManager;
        networkManager = nullptr;
    }
}

ApiClient* ApiClient::getInstance() {
    if (instance == nullptr) {
        instance = new ApiClient();
    }
    return instance;
}

void ApiClient::setServerUrl(const QString& url) {
    if (url.isEmpty()) {
        return;
    }

    baseUrl = url;
    if (!baseUrl.endsWith("/")) {
        baseUrl += "/";
    }
    emit serverUrlChanged(baseUrl);
}

QString ApiClient::normalizeUrl(const QString& path) {
    QString url = baseUrl;
    if (url.endsWith("/")) {
        url.chop(1);
    }
    QString cleanPath = path;
    if (cleanPath.startsWith("/")) {
        cleanPath = cleanPath.mid(1);
    }
    return url + "/" + cleanPath;
}

void ApiClient::getAllAnime(int page, int limit) {
    if (!networkManager) {
        emit errorOccurred("Network manager not initialized");
        return;
    }

    QString fullPath = normalizeUrl(QString("anime?page=%1&limit=%2").arg(page).arg(limit));
    QUrl url(fullPath);
    QNetworkRequest request(url);
    networkManager->get(request);
}

void ApiClient::getAnimeById(int id) {
    if (!networkManager) {
        emit errorOccurred("Network manager not initialized");
        return;
    }

    QString fullPath = normalizeUrl(QString("anime/%1").arg(id));
    QUrl url(fullPath);
    QNetworkRequest request(url);
    networkManager->get(request);
}

void ApiClient::createAnime(const Anime& anime) {
    if (!networkManager) {
        emit errorOccurred("Network manager not initialized");
        return;
    }

    QString fullPath = normalizeUrl("anime");
    QUrl url(fullPath);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = AnimeAdapter::toJson(anime);
    QJsonDocument doc(json);

    networkManager->post(request, doc.toJson());
}

void ApiClient::updateAnime(int id, const Anime& anime) {
    if (!networkManager) {
        emit errorOccurred("Network manager not initialized");
        return;
    }

    QString fullPath = normalizeUrl(QString("anime/%1").arg(id));
    QUrl url(fullPath);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");

    QJsonObject json = AnimeAdapter::toJson(anime);
    QJsonDocument doc(json);

    networkManager->put(request, doc.toJson());
}

void ApiClient::deleteAnime(int id) {
    if (!networkManager) {
        emit errorOccurred("Network manager not initialized");
        return;
    }

    QString fullPath = normalizeUrl(QString("anime/%1").arg(id));
    QUrl url(fullPath);
    QNetworkRequest request(url);
    networkManager->deleteResource(request);
}

void ApiClient::getStats() {
    if (!networkManager) {
        emit errorOccurred("Network manager not initialized");
        return;
    }

    QString fullPath = normalizeUrl("stats");
    QUrl url(fullPath);
    QNetworkRequest request(url);
    networkManager->get(request);
}

void ApiClient::onReplyFinished(QNetworkReply* reply) {
    if (!reply) {
        emit errorOccurred("Null reply received");
        return;
    }

    QVariant redirect = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
    if (!redirect.isNull()) {
        QUrl redirectUrl = redirect.toUrl();

        if (reply->operation() == QNetworkAccessManager::PostOperation) {
            QNetworkRequest newRequest(redirectUrl);
            newRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            networkManager->post(newRequest, QByteArray());
        }
        else if (reply->operation() == QNetworkAccessManager::GetOperation) {
            QNetworkRequest newRequest(redirectUrl);
            networkManager->get(newRequest);
        }
        else if (reply->operation() == QNetworkAccessManager::PutOperation) {
            QNetworkRequest newRequest(redirectUrl);
            newRequest.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
            networkManager->put(newRequest, QByteArray());
        }
        else if (reply->operation() == QNetworkAccessManager::DeleteOperation) {
            QNetworkRequest newRequest(redirectUrl);
            networkManager->deleteResource(newRequest);
        }

        reply->deleteLater();
        return;
    }

    QNetworkAccessManager::Operation operation = reply->operation();
    QString urlPath = reply->url().path();

    if (operation == QNetworkAccessManager::DeleteOperation) {
        if (reply->error() == QNetworkReply::NoError) {
            int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
            if (statusCode == 204 || statusCode == 200) {
                if (urlPath.contains("/anime/")) {
                    int id = urlPath.split("/").last().toInt();
                    emit animeDeleted(id);
                }
            }
        } else {
            emit errorOccurred(QString("Delete error: %1").arg(reply->errorString()));
        }
        reply->deleteLater();
        return;
    }

    if (operation == QNetworkAccessManager::PostOperation) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();

            if (!responseData.isEmpty()) {
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

                if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("id")) {
                        Anime anime = AnimeAdapter::fromJson(obj);
                        emit animeCreated(anime);
                    } else {
                        emit errorOccurred("Server response missing 'id' field");
                    }
                } else {
                    emit errorOccurred("Invalid JSON response from server");
                }
            } else {
                emit errorOccurred("Server returned empty response");
            }
        } else {
            emit errorOccurred(QString("POST error: %1").arg(reply->errorString()));
        }
        reply->deleteLater();
        return;
    }

    if (operation == QNetworkAccessManager::PutOperation) {
        if (reply->error() == QNetworkReply::NoError) {
            QByteArray responseData = reply->readAll();

            if (!responseData.isEmpty()) {
                QJsonParseError parseError;
                QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

                if (parseError.error == QJsonParseError::NoError && doc.isObject()) {
                    QJsonObject obj = doc.object();
                    if (obj.contains("id")) {
                        Anime anime = AnimeAdapter::fromJson(obj);
                        emit animeUpdated(anime);
                    }
                }
            }
        } else {
            emit errorOccurred(QString("PUT error: %1").arg(reply->errorString()));
        }
        reply->deleteLater();
        return;
    }

    if (reply->error() != QNetworkReply::NoError) {
        emit errorOccurred(QString("Network error: %1").arg(reply->errorString()));
        reply->deleteLater();
        return;
    }

    QByteArray responseData = reply->readAll();

    if (responseData.isEmpty()) {
        reply->deleteLater();
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(responseData, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        emit errorOccurred(QString("JSON parse error: %1").arg(parseError.errorString()));
        reply->deleteLater();
        return;
    }

    if (urlPath == "/anime" && operation == QNetworkAccessManager::GetOperation) {
        AnimeListResponse response = AnimeAdapter::fromListResponse(doc);
        emit animeListReceived(response);
    }
    else if (urlPath.contains("/anime/") && urlPath.length() > 7 &&
             operation == QNetworkAccessManager::GetOperation) {
        if (doc.isObject()) {
            Anime anime = AnimeAdapter::fromJson(doc.object());
            emit animeReceived(anime);
        }
    }
    else if (urlPath == "/stats") {
        if (doc.isObject()) {
            emit statsReceived(doc.object());
        }
    }

    reply->deleteLater();
}
