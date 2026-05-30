#include "animeadapter.h"
#include <QDebug>

// Десериализация одного аниме
Anime AnimeAdapter::fromJson(const QJsonObject& json) {
    Anime anime;

    // Используем QJsonValue для извлечения полей
    if (json.contains("id") && json["id"].isDouble())
        anime.setId(json["id"].toInt());

    if (json.contains("title") && json["title"].isString())
        anime.setTitle(json["title"].toString());

    if (json.contains("synopsis") && json["synopsis"].isString())
        anime.setSynopsis(json["synopsis"].toString());

    if (json.contains("genres") && json["genres"].isString())
        anime.setGenres(json["genres"].toString());

    if (json.contains("studios") && json["studios"].isString())
        anime.setStudios(json["studios"].toString());

    if (json.contains("episodes") && json["episodes"].isDouble())
        anime.setEpisodes(json["episodes"].toInt());

    if (json.contains("image_webp") && json["image_webp"].isString())
        anime.setImageWebp(json["image_webp"].toString());

    if (json.contains("trailer_url") && json["trailer_url"].isString())
        anime.setTrailerUrl(json["trailer_url"].toString());

    if (json.contains("year") && json["year"].isDouble())
        anime.setYear(json["year"].toInt());

    if (json.contains("themes") && json["themes"].isString())
        anime.setThemes(json["themes"].toString());

    if (json.contains("demographics") && json["demographics"].isString())
        anime.setDemographics(json["demographics"].toString());

    if (json.contains("rating") && json["rating"].isString())
        anime.setRating(json["rating"].toString());

    if (json.contains("duration") && json["duration"].isString())
        anime.setDuration(json["duration"].toString());

    return anime;
}

// Десериализация массива
QList<Anime> AnimeAdapter::fromJsonArray(const QJsonArray& jsonArray) {
    QList<Anime> animeList;

    for (const QJsonValue& value : jsonArray) {
        if (value.isObject()) {
            Anime anime = fromJson(value.toObject());
            animeList.append(anime);
        }
    }

    return animeList;
}

// Десериализация ответа {"total": X, "items": [...]}
AnimeListResponse AnimeAdapter::fromListResponse(const QJsonDocument& doc) {
    AnimeListResponse response;

    if (!doc.isObject()) {
        qWarning() << "Invalid JSON document";
        return response;
    }

    QJsonObject root = doc.object();

    // Извлекаем total
    if (root.contains("total") && root["total"].isDouble()) {
        response.total = root["total"].toInt();
    }

    // Извлекаем items
    if (root.contains("items") && root["items"].isArray()) {
        response.items = fromJsonArray(root["items"].toArray());
    }

    return response;
}

// Сериализация C++ объекта в JSON
QJsonObject AnimeAdapter::toJson(const Anime& anime) {
    QJsonObject json;

    json["title"] = anime.getTitle();
    json["synopsis"] = anime.getSynopsis();
    json["genres"] = anime.getGenres();
    json["studios"] = anime.getStudios();
    json["episodes"] = anime.getEpisodes();
    json["image_webp"] = anime.getImageWebp();
    json["trailer_url"] = anime.getTrailerUrl();
    json["year"] = anime.getYear();
    json["themes"] = anime.getThemes();
    json["demographics"] = anime.getDemographics();
    json["rating"] = anime.getRating();
    json["duration"] = anime.getDuration();

    return json;
}
