#include "consoleclient.h"
#include <QTimer>
#include <QCoreApplication>
#include <QDebug>

ConsoleClient::ConsoleClient() {
    // Подключаемся к сигналам Singleton'а
    ApiClient* client = ApiClient::getInstance();

    connect(client, &ApiClient::animeListReceived,
            this, &ConsoleClient::onAnimeListReceived);
    connect(client, &ApiClient::animeReceived,
            this, &ConsoleClient::onAnimeReceived);
    connect(client, &ApiClient::animeCreated,
            this, &ConsoleClient::onAnimeCreated);
    connect(client, &ApiClient::animeUpdated,
            this, &ConsoleClient::onAnimeUpdated);
    connect(client, &ApiClient::animeDeleted,
            this, &ConsoleClient::onAnimeDeleted);
    connect(client, &ApiClient::statsReceived,
            this, &ConsoleClient::onStatsReceived);
    connect(client, &ApiClient::errorOccurred,
            this, &ConsoleClient::onError);
}

void ConsoleClient::run() {
    ApiClient* client = ApiClient::getInstance();

    qDebug().noquote() << "\n========== АНИМЕ API КЛИЕНТ ==========\n";

    // 1. Получаем статистику
    qDebug() << "Запрос статистики...";
    client->getStats();

    // 2. Получаем список аниме (со страницы 1, 5 элементов)
    QTimer::singleShot(2000, [=]() {
        qDebug() << "\nЗапрос списка аниме...";
        client->getAllAnime(1, 5);
    });

    // 3. Получаем конкретное аниме по ID
    QTimer::singleShot(4000, [=]() {
        qDebug() << "\nЗапрос аниме с ID 1...";
        client->getAnimeById(1);
    });

    // 4. Создаем новое аниме
    QTimer::singleShot(6000, [=]() {
        qDebug() << "\nСоздание нового аниме...";
        Anime newAnime;
        newAnime.setTitle("Атака Титанов");
        newAnime.setGenres("Экшен, Драма, Фэнтези");
        newAnime.setStudios("Wit Studio");
        newAnime.setEpisodes(25);
        newAnime.setYear(2013);
        newAnime.setRating("R+");
        client->createAnime(newAnime);
    });

    // 5. Обновляем аниме
    QTimer::singleShot(8000, [=]() {
        qDebug() << "\nОбновление аниме...";
        Anime updatedAnime;
        updatedAnime.setTitle("Атака Титанов (Обновлено)");
        updatedAnime.setGenres("Экшен, Драма, Фэнтези, Сёнен");
        updatedAnime.setStudios("Wit Studio, MAPPA");
        updatedAnime.setEpisodes(75);
        updatedAnime.setYear(2013);
        client->updateAnime(1, updatedAnime);
    });

    // 6. Удаляем аниме
    // QTimer::singleShot(10000, [=]() {
    //     qDebug() << "\nУдаление аниме с ID 2";
    //     client->deleteAnime(2);
    // });

    // Завершаем приложение через 12 секунд
    QTimer::singleShot(12000, [=]() {
        qDebug() << "\nЗавершение работы";
        QCoreApplication::quit();
    });
}

void ConsoleClient::onAnimeListReceived(const AnimeListResponse& response) {
    qDebug().noquote() << "\nПолучен список аниме:";
    qDebug().noquote() << "Всего записей:" << response.total;
    qDebug().noquote() << "Загружено:" << response.items.size();

    for (const auto& anime : response.items) {
        anime.print();
    }
}

void ConsoleClient::onAnimeReceived(const Anime& anime) {
    qDebug().noquote() << "\nПолучено аниме:";
    anime.print();
}

void ConsoleClient::onAnimeCreated(const Anime& anime) {
    qDebug().noquote() << "\nСоздано новое аниме:";
    anime.print();
}

void ConsoleClient::onAnimeUpdated(const Anime& anime) {
    qDebug().noquote() << "\nОбновлено аниме:";
    anime.print();
}

void ConsoleClient::onAnimeDeleted(int id) {
    qDebug().noquote() << "\nУдалено аниме с ID:" << id;
}

void ConsoleClient::onStatsReceived(const QJsonObject& stats) {
    qDebug().noquote() << "\nСтатистика сервера:";
    qDebug().noquote() << "Всего аниме:" << stats.value("total_anime").toInt();
    qDebug().noquote() << "Всего годов:" << stats.value("total_years").toInt();
}

void ConsoleClient::onError(const QString& error) {
    qCritical().noquote() << "\nОШИБКА:" << error;
}
