#include "anime.h"
#include <QDebug>

Anime::Anime() : id(0), episodes(0), year(0) {}

Anime::Anime(int id, const QString& title, const QString& synopsis)
    : id(id), title(title), synopsis(synopsis), episodes(0), year(0) {}

void Anime::print() const {
    qDebug().noquote() << "====================";
    qDebug().noquote() << "ID:" << id;
    qDebug().noquote() << "Title:" << title;
    if (!synopsis.isEmpty())
        qDebug().noquote() << "Synopsis:" << synopsis.left(100) + "...";
    if (!genres.isEmpty())
        qDebug().noquote() << "Genres:" << genres;
    if (!studios.isEmpty())
        qDebug().noquote() << "Studios:" << studios;
    if (episodes > 0)
        qDebug().noquote() << "Episodes:" << episodes;
    if (year > 0)
        qDebug().noquote() << "Year:" << year;
    if (!rating.isEmpty())
        qDebug().noquote() << "Rating:" << rating;
    qDebug().noquote() << "====================";
}
