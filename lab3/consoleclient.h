#ifndef CONSOLECLIENT_H
#define CONSOLECLIENT_H

#include <QObject>
#include <QJsonObject>
#include "apiclient.h"

class ConsoleClient : public QObject {
    Q_OBJECT

public:
    ConsoleClient();
    void run();

private slots:
    void onAnimeListReceived(const AnimeListResponse& response);
    void onAnimeReceived(const Anime& anime);
    void onAnimeCreated(const Anime& anime);
    void onAnimeUpdated(const Anime& anime);
    void onAnimeDeleted(int id);
    void onStatsReceived(const QJsonObject& stats);
    void onError(const QString& error);
};

#endif // CONSOLECLIENT_H
