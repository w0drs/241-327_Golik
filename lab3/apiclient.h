#ifndef APICLIENT_H
#define APICLIENT_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QJsonObject>
#include <QSslConfiguration>
#include <QMutex>
#include "anime.h"

class ApiClient : public QObject {
    Q_OBJECT

private:
    explicit ApiClient(QObject *parent = nullptr);
    ~ApiClient();

    ApiClient(const ApiClient&) = delete;
    ApiClient& operator=(const ApiClient&) = delete;

    static ApiClient* instance;
    static QMutex mutex;

    QNetworkAccessManager* networkManager;
    QString baseUrl;

    QString normalizeUrl(const QString& path);

public:
    static ApiClient* getInstance();

    void setServerUrl(const QString& url);
    QString getServerUrl() const { return baseUrl; }

    void getAllAnime(int page = 1, int limit = 20);
    void getAnimeById(int id);
    void createAnime(const Anime& anime);
    void updateAnime(int id, const Anime& anime);
    void deleteAnime(int id);
    void getStats();

signals:
    void animeListReceived(const AnimeListResponse& response);
    void animeReceived(const Anime& anime);
    void animeCreated(const Anime& anime);
    void animeUpdated(const Anime& anime);
    void animeDeleted(int id);
    void statsReceived(const QJsonObject& stats);
    void errorOccurred(const QString& error);
    void serverUrlChanged(const QString& newUrl);

private slots:
    void onReplyFinished(QNetworkReply* reply);
};

#endif // APICLIENT_H
