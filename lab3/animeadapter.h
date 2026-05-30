#ifndef ANIMEADAPTER_H
#define ANIMEADAPTER_H

#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>
#include "anime.h"

class AnimeAdapter {
public:
    // Адаптируем JSON в объект Anime
    static Anime fromJson(const QJsonObject& json);

    // Адаптируем JSON массив в список Anime
    static QList<Anime> fromJsonArray(const QJsonArray& jsonArray);

    // Адаптируем полный ответ сервера (с пагинацией)
    static AnimeListResponse fromListResponse(const QJsonDocument& doc);

    // Обратное преобразование (для POST/PUT)
    static QJsonObject toJson(const Anime& anime);
};

#endif // ANIMEADAPTER_H
