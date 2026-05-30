#ifndef ANIME_H
#define ANIME_H

#include <QString>
#include <QList>

class Anime {
private:
    int id;
    QString title;
    QString synopsis;
    QString genres;
    QString studios;
    int episodes;
    QString imageWebp;
    QString trailerUrl;
    int year;
    QString themes;
    QString demographics;
    QString rating;
    QString duration;

public:
    // Конструкторы
    Anime();
    Anime(int id, const QString& title, const QString& synopsis);

    // Getters
    int getId() const { return id; }
    QString getTitle() const { return title; }
    QString getSynopsis() const { return synopsis; }
    QString getGenres() const { return genres; }
    QString getStudios() const { return studios; }
    int getEpisodes() const { return episodes; }
    QString getImageWebp() const { return imageWebp; }
    QString getTrailerUrl() const { return trailerUrl; }
    int getYear() const { return year; }
    QString getThemes() const { return themes; }
    QString getDemographics() const { return demographics; }
    QString getRating() const { return rating; }
    QString getDuration() const { return duration; }

    // Setters
    void setId(int id) { this->id = id; }
    void setTitle(const QString& title) { this->title = title; }
    void setSynopsis(const QString& synopsis) { this->synopsis = synopsis; }
    void setGenres(const QString& genres) { this->genres = genres; }
    void setStudios(const QString& studios) { this->studios = studios; }
    void setEpisodes(int episodes) { this->episodes = episodes; }
    void setImageWebp(const QString& imageWebp) { this->imageWebp = imageWebp; }
    void setTrailerUrl(const QString& trailerUrl) { this->trailerUrl = trailerUrl; }
    void setYear(int year) { this->year = year; }
    void setThemes(const QString& themes) { this->themes = themes; }
    void setDemographics(const QString& demographics) { this->demographics = demographics; }
    void setRating(const QString& rating) { this->rating = rating; }
    void setDuration(const QString& duration) { this->duration = duration; }

    // Вспомогательные методы
    void print() const;  // Вывод в консоль
};

// Структура для списка аниме (ответ от сервера)
struct AnimeListResponse {
    int total;
    QList<Anime> items;
};

#endif // ANIME_H
