#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QHeaderView>
#include <QDateTime>
#include <QSslConfiguration>
#include <QSslSocket>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , currentTotal(0)
{
    ui->setupUi(this);

    // Настройка таблицы
    ui->animeTable->setColumnCount(13);
    QStringList headers;
    headers << "ID" << "Title" << "Genres" << "Studios" << "Episodes"
            << "Year" << "Rating" << "Synopsis" << "Themes" << "Demographics"
            << "Duration" << "Image URL" << "Trailer URL";
    ui->animeTable->setHorizontalHeaderLabels(headers);
    ui->animeTable->horizontalHeader()->setStretchLastSection(true);

    ui->animeTable->setColumnWidth(0, 50);
    ui->animeTable->setColumnWidth(1, 200);
    ui->animeTable->setColumnWidth(2, 150);
    ui->animeTable->setColumnWidth(3, 150);
    ui->animeTable->setColumnWidth(4, 60);
    ui->animeTable->setColumnWidth(5, 60);
    ui->animeTable->setColumnWidth(6, 60);
    ui->animeTable->setColumnWidth(7, 200);
    ui->animeTable->setColumnWidth(8, 100);
    ui->animeTable->setColumnWidth(9, 100);
    ui->animeTable->setColumnWidth(10, 80);
    ui->animeTable->setColumnWidth(11, 150);
    ui->animeTable->setColumnWidth(12, 150);

    ui->animeTable->setAlternatingRowColors(true);
    ui->animeTable->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->animeTable->setSelectionMode(QAbstractItemView::SingleSelection);

    // Отключаем проверку SSL для самоподписанного сертификата
    QSslConfiguration sslConfig = QSslConfiguration::defaultConfiguration();
    sslConfig.setPeerVerifyMode(QSslSocket::VerifyNone);
    QSslConfiguration::setDefaultConfiguration(sslConfig);

    // Создаем API клиент
    apiClient = new OpenAPI::OAIDefaultApi(3000);
    apiClient->setParent(this);

    apiClient->setNewServerForAllOperations(QUrl("https://localhost"));

    setupConnections();

    appendToOutput("=== Anime REST Client Started ===");
    appendToOutput("Using generated OpenAPI client (signals)");
    appendToOutput("Server URL: https://localhost (via nginx)");
    appendToOutput("SSL verification disabled for self-signed certificate");
    appendToOutput("Click LIST button to fetch anime from server");
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setupConnections()
{
    // Подключаем кнопки
    connect(ui->btnList, &QPushButton::clicked, this, &MainWindow::onListClicked);
    connect(ui->btnGet, &QPushButton::clicked, this, &MainWindow::onGetClicked);
    connect(ui->btnCreate, &QPushButton::clicked, this, &MainWindow::onCreateClicked);
    connect(ui->btnUpdate, &QPushButton::clicked, this, &MainWindow::onUpdateClicked);
    connect(ui->btnDelete, &QPushButton::clicked, this, &MainWindow::onDeleteClicked);
    connect(ui->btnConnect, &QPushButton::clicked, this, &MainWindow::onConnectClicked);
    connect(ui->btnPrevPage, &QPushButton::clicked, this, &MainWindow::onPrevPageClicked);
    connect(ui->btnNextPage, &QPushButton::clicked, this, &MainWindow::onNextPageClicked);
    connect(ui->btnRefresh, &QPushButton::clicked, this, &MainWindow::onRefreshClicked);
    connect(ui->spinPage, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onPageChanged);
    connect(ui->comboLimit, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onLimitChanged);
    connect(ui->actionExit, &QAction::triggered, this, &MainWindow::onExitTriggered);
    connect(ui->actionAbout, &QAction::triggered, this, &MainWindow::onAboutTriggered);

    // Сигнал успешного получения списка аниме
    connect(apiClient, &OpenAPI::OAIDefaultApi::listAnimeAnimeGetSignal,
            this, [this](OpenAPI::OAIAnimeListResponse response) {
                currentTotal = response.getTotal();
                int currentLimit = ui->comboLimit->currentText().toInt();
                int totalPages = (currentTotal + currentLimit - 1) / currentLimit;

                appendToOutput(QString("Received %1 items, Total: %2")
                                   .arg(response.getItems().size())
                                   .arg(response.getTotal()));

                ui->labelTotalInfo->setText(QString("Total: %1 items | Page %2 of %3")
                                                .arg(currentTotal)
                                                .arg(ui->spinPage->value())
                                                .arg(totalPages > 0 ? totalPages : 1));
                ui->spinPage->setMaximum(totalPages > 0 ? totalPages : 1);

                ui->animeTable->setRowCount(0);
                const auto& items = response.getItems();
                for (int i = 0; i < items.size(); ++i) {
                    const auto& anime = items[i];
                    ui->animeTable->insertRow(i);
                    ui->animeTable->setItem(i, 0, new QTableWidgetItem(QString::number(anime.getId())));
                    ui->animeTable->setItem(i, 1, new QTableWidgetItem(anime.getTitle()));
                    ui->animeTable->setItem(i, 2, new QTableWidgetItem(anime.getGenres().isNull() ? "N/A" : anime.getGenres()));
                    ui->animeTable->setItem(i, 3, new QTableWidgetItem(anime.getStudios().isNull() ? "N/A" : anime.getStudios()));
                    ui->animeTable->setItem(i, 4, new QTableWidgetItem(anime.getEpisodes() > 0 ? QString::number(anime.getEpisodes()) : "N/A"));
                    ui->animeTable->setItem(i, 5, new QTableWidgetItem(anime.getYear() > 0 ? QString::number(anime.getYear()) : "N/A"));
                    ui->animeTable->setItem(i, 6, new QTableWidgetItem(anime.getRating().isNull() ? "N/A" : anime.getRating()));
                    ui->animeTable->setItem(i, 7, new QTableWidgetItem(anime.getSynopsis().isNull() ? "N/A" : anime.getSynopsis().left(100)));
                    ui->animeTable->setItem(i, 8, new QTableWidgetItem(anime.getThemes().isNull() ? "N/A" : anime.getThemes()));
                    ui->animeTable->setItem(i, 9, new QTableWidgetItem(anime.getDemographics().isNull() ? "N/A" : anime.getDemographics()));
                    ui->animeTable->setItem(i, 10, new QTableWidgetItem(anime.getDuration().isNull() ? "N/A" : anime.getDuration()));
                    ui->animeTable->setItem(i, 11, new QTableWidgetItem(anime.getImageWebp().isNull() ? "N/A" : anime.getImageWebp()));
                    ui->animeTable->setItem(i, 12, new QTableWidgetItem(anime.getTrailerUrl().isNull() ? "N/A" : anime.getTrailerUrl()));
                }
            });

    // Сигнал успешного получения одного аниме
    connect(apiClient, &OpenAPI::OAIDefaultApi::getAnimeAnimeAnimeIdGetSignal,
            this, [this](OpenAPI::OAIAnime anime) {
                appendToOutput(QString("========================================"));
                appendToOutput(QString("Anime found:"));
                appendToOutput(QString("  ID: %1").arg(anime.getId()));
                appendToOutput(QString("  Title: %1").arg(anime.getTitle()));
                appendToOutput(QString("  Synopsis: %1").arg(anime.getSynopsis().isNull() ? "N/A" : anime.getSynopsis().left(200)));
                appendToOutput(QString("  Genres: %1").arg(anime.getGenres().isNull() ? "N/A" : anime.getGenres()));
                appendToOutput(QString("  Studios: %1").arg(anime.getStudios().isNull() ? "N/A" : anime.getStudios()));
                appendToOutput(QString("  Episodes: %1").arg(anime.getEpisodes() > 0 ? QString::number(anime.getEpisodes()) : "N/A"));
                appendToOutput(QString("  Year: %1").arg(anime.getYear() > 0 ? QString::number(anime.getYear()) : "N/A"));
                appendToOutput(QString("  Rating: %1").arg(anime.getRating().isNull() ? "N/A" : anime.getRating()));
                appendToOutput(QString("  Themes: %1").arg(anime.getThemes().isNull() ? "N/A" : anime.getThemes()));
                appendToOutput(QString("  Demographics: %1").arg(anime.getDemographics().isNull() ? "N/A" : anime.getDemographics()));
                appendToOutput(QString("  Duration: %1").arg(anime.getDuration().isNull() ? "N/A" : anime.getDuration()));
                if (!anime.getImageWebp().isNull()) {
                    appendToOutput(QString("  Image URL: %1").arg(anime.getImageWebp()));
                }
                if (!anime.getTrailerUrl().isNull()) {
                    appendToOutput(QString("  Trailer URL: %1").arg(anime.getTrailerUrl()));
                }
                appendToOutput(QString("========================================"));

                for (int i = 0; i < ui->animeTable->rowCount(); ++i) {
                    if (ui->animeTable->item(i, 0)->text().toInt() == anime.getId()) {
                        ui->animeTable->selectRow(i);
                        ui->animeTable->scrollToItem(ui->animeTable->item(i, 0));
                        break;
                    }
                }
            });

    // Сигнал успешного создания аниме
    connect(apiClient, &OpenAPI::OAIDefaultApi::createAnimeAnimePostSignal,
            this, [this](OpenAPI::OAIAnime result) {
                appendToOutput(QString("========================================"));
                appendToOutput(QString("SUCCESS: Anime created successfully!"));
                appendToOutput(QString("  New ID: %1").arg(result.getId()));
                appendToOutput(QString("  Title: %1").arg(result.getTitle()));
                appendToOutput(QString("  Genres: %1").arg(result.getGenres().isNull() ? "N/A" : result.getGenres()));
                appendToOutput(QString("  Studios: %1").arg(result.getStudios().isNull() ? "N/A" : result.getStudios()));
                appendToOutput(QString("  Episodes: %1").arg(result.getEpisodes()));
                appendToOutput(QString("  Year: %1").arg(result.getYear()));
                appendToOutput(QString("  Rating: %1").arg(result.getRating().isNull() ? "N/A" : result.getRating()));
                if (!result.getSynopsis().isNull()) {
                    appendToOutput(QString("  Synopsis: %1").arg(result.getSynopsis().left(100)));
                }
                appendToOutput(QString("========================================"));

                ui->titleInput->clear();
                ui->genresInput->clear();
                ui->studiosInput->clear();
                ui->episodesInput->clear();
                ui->yearInput->clear();
                ui->ratingInput->clear();
            });

    // Сигнал успешного обновления аниме
    connect(apiClient, &OpenAPI::OAIDefaultApi::updateAnimeAnimeAnimeIdPutSignal,
            this, [this](OpenAPI::OAIAnime result) {
                appendToOutput(QString("========================================"));
                appendToOutput(QString("SUCCESS: Anime updated successfully"));
                appendToOutput(QString("  ID: %1").arg(result.getId()));
                appendToOutput(QString("  Title: %1").arg(result.getTitle()));
                appendToOutput(QString("  Genres: %1").arg(result.getGenres().isNull() ? "N/A" : result.getGenres()));
                appendToOutput(QString("  Studios: %1").arg(result.getStudios().isNull() ? "N/A" : result.getStudios()));
                appendToOutput(QString("  Episodes: %1").arg(result.getEpisodes()));
                appendToOutput(QString("  Year: %1").arg(result.getYear()));
                appendToOutput(QString("  Rating: %1").arg(result.getRating().isNull() ? "N/A" : result.getRating()));
                appendToOutput(QString("========================================"));
            });

    // Сигнал успешного удаления аниме
    connect(apiClient, &OpenAPI::OAIDefaultApi::deleteAnimeAnimeAnimeIdDeleteSignal,
            this, [this]() {
                appendToOutput(QString("========================================"));
                appendToOutput(QString("SUCCESS: Anime deleted successfully"));
                appendToOutput(QString("========================================"));
                ui->idInput->clear();
            });

    // Сигналы ошибок
    connect(apiClient, &OpenAPI::OAIDefaultApi::listAnimeAnimeGetSignalE,
            this, [this](OpenAPI::OAIAnimeListResponse summary, QNetworkReply::NetworkError error_type, QString error_str) {
                appendToOutput(QString("ERROR: %1").arg(error_str));
                QMessageBox::warning(this, "Error", error_str);
            });

    connect(apiClient, &OpenAPI::OAIDefaultApi::getAnimeAnimeAnimeIdGetSignalE,
            this, [this](OpenAPI::OAIAnime summary, QNetworkReply::NetworkError error_type, QString error_str) {
                appendToOutput(QString("ERROR: %1").arg(error_str));
                QMessageBox::warning(this, "Error", error_str);
            });

    connect(apiClient, &OpenAPI::OAIDefaultApi::createAnimeAnimePostSignalE,
            this, [this](OpenAPI::OAIAnime summary, QNetworkReply::NetworkError error_type, QString error_str) {
                appendToOutput(QString("CREATE ERROR: %1").arg(error_str));
                QMessageBox::warning(this, "Error", error_str);
            });

    connect(apiClient, &OpenAPI::OAIDefaultApi::updateAnimeAnimeAnimeIdPutSignalE,
            this, [this](OpenAPI::OAIAnime summary, QNetworkReply::NetworkError error_type, QString error_str) {
                appendToOutput(QString("UPDATE ERROR: %1").arg(error_str));
                QMessageBox::warning(this, "Error", error_str);
            });

    connect(apiClient, &OpenAPI::OAIDefaultApi::deleteAnimeAnimeAnimeIdDeleteSignalE,
            this, [this](QNetworkReply::NetworkError error_type, QString error_str) {
                appendToOutput(QString("DELETE ERROR: %1").arg(error_str));
                QMessageBox::warning(this, "Error", error_str);
            });
}

void MainWindow::appendToOutput(const QString& text)
{
    QString timestamp = QDateTime::currentDateTime().toString("hh:mm:ss.zzz");
    ui->outputText->append(QString("[%1] %2").arg(timestamp, text));
    QTextCursor cursor = ui->outputText->textCursor();
    cursor.movePosition(QTextCursor::End);
    ui->outputText->setTextCursor(cursor);
}

void MainWindow::onListClicked()
{
    loadCurrentPage();
}

void MainWindow::loadCurrentPage()
{
    int page = ui->spinPage->value();
    int limit = ui->comboLimit->currentText().toInt();

    appendToOutput(QString("[GET /anime] page=%1, limit=%2").arg(page).arg(limit));
    apiClient->listAnimeAnimeGet(page, limit);
}

void MainWindow::onGetClicked()
{
    if (ui->idInput->text().isEmpty()) {
        appendToOutput("ERROR: Please enter an ID");
        QMessageBox::warning(this, "Input Error", "Please enter anime ID");
        return;
    }

    bool ok;
    int id = ui->idInput->text().toInt(&ok);
    if (!ok || id <= 0) {
        appendToOutput("ERROR: Please enter a valid positive integer ID");
        QMessageBox::warning(this, "Input Error", "Please enter a valid positive integer ID");
        return;
    }

    appendToOutput(QString("[GET /anime/%1] Sending request...").arg(id));
    apiClient->getAnimeAnimeAnimeIdGet(id);
}

void MainWindow::onCreateClicked()
{
    if (ui->titleInput->text().isEmpty()) {
        appendToOutput("ERROR: Title required for creating anime");
        QMessageBox::warning(this, "Input Error", "Please enter anime title");
        return;
    }

    OpenAPI::OAIAnimeCreate newAnime;
    newAnime.setTitle(ui->titleInput->text());
    newAnime.setSynopsis("");
    newAnime.setGenres(ui->genresInput->text());
    newAnime.setStudios(ui->studiosInput->text());
    newAnime.setEpisodes(ui->episodesInput->text().toInt());
    newAnime.setYear(ui->yearInput->text().toInt());
    newAnime.setRating(ui->ratingInput->text());

    appendToOutput(QString("[POST /anime] Creating new anime: %1").arg(ui->titleInput->text()));
    appendToOutput(QString("  Data: Title=%1, Genres=%2, Studios=%3, Episodes=%4, Year=%5, Rating=%6")
                       .arg(ui->titleInput->text())
                       .arg(ui->genresInput->text().isEmpty() ? "(empty)" : ui->genresInput->text())
                       .arg(ui->studiosInput->text().isEmpty() ? "(empty)" : ui->studiosInput->text())
                       .arg(ui->episodesInput->text().isEmpty() ? "0" : ui->episodesInput->text())
                       .arg(ui->yearInput->text().isEmpty() ? "0" : ui->yearInput->text())
                       .arg(ui->ratingInput->text().isEmpty() ? "(empty)" : ui->ratingInput->text()));

    apiClient->createAnimeAnimePost(newAnime);
}

void MainWindow::onUpdateClicked()
{
    if (ui->idInput->text().isEmpty()) {
        appendToOutput("ERROR: Please enter an ID to update");
        QMessageBox::warning(this, "Input Error", "Please enter anime ID");
        return;
    }

    if (ui->titleInput->text().isEmpty()) {
        appendToOutput("ERROR: Title required for updating");
        QMessageBox::warning(this, "Input Error", "Please enter anime title");
        return;
    }

    bool ok;
    int id = ui->idInput->text().toInt(&ok);
    if (!ok || id <= 0) {
        appendToOutput("ERROR: Please enter a valid positive integer ID");
        QMessageBox::warning(this, "Input Error", "Please enter a valid positive integer ID");
        return;
    }

    OpenAPI::OAIAnimeUpdate updatedAnime;
    updatedAnime.setTitle(ui->titleInput->text());
    updatedAnime.setSynopsis("");
    updatedAnime.setGenres(ui->genresInput->text());
    updatedAnime.setStudios(ui->studiosInput->text());
    updatedAnime.setEpisodes(ui->episodesInput->text().toInt());
    updatedAnime.setYear(ui->yearInput->text().toInt());
    updatedAnime.setRating(ui->ratingInput->text());

    appendToOutput(QString("[PUT /anime/%1] Updating anime...").arg(id));
    appendToOutput(QString("  New Data: Title=%1, Genres=%2, Studios=%3, Episodes=%4, Year=%5, Rating=%6")
                       .arg(ui->titleInput->text())
                       .arg(ui->genresInput->text().isEmpty() ? "(empty)" : ui->genresInput->text())
                       .arg(ui->studiosInput->text().isEmpty() ? "(empty)" : ui->studiosInput->text())
                       .arg(ui->episodesInput->text().isEmpty() ? "0" : ui->episodesInput->text())
                       .arg(ui->yearInput->text().isEmpty() ? "0" : ui->yearInput->text())
                       .arg(ui->ratingInput->text().isEmpty() ? "(empty)" : ui->ratingInput->text()));

    apiClient->updateAnimeAnimeAnimeIdPut(id, updatedAnime);
}

void MainWindow::onDeleteClicked()
{
    if (ui->idInput->text().isEmpty()) {
        appendToOutput("ERROR: Please enter an ID to delete");
        QMessageBox::warning(this, "Input Error", "Please enter anime ID");
        return;
    }

    bool ok;
    int id = ui->idInput->text().toInt(&ok);
    if (!ok || id <= 0) {
        appendToOutput("ERROR: Please enter a valid positive integer ID");
        QMessageBox::warning(this, "Input Error", "Please enter a valid positive integer ID");
        return;
    }

    QMessageBox::StandardButton reply = QMessageBox::question(
        this, "Confirm Delete",
        QString("Are you sure you want to delete anime with ID %1?").arg(id),
        QMessageBox::Yes | QMessageBox::No);

    if (reply == QMessageBox::Yes) {
        appendToOutput(QString("[DELETE /anime/%1] Sending request...").arg(id));
        apiClient->deleteAnimeAnimeAnimeIdDelete(id);
    } else {
        appendToOutput("Delete cancelled by user");
    }
}

void MainWindow::onPrevPageClicked()
{
    if (ui->spinPage->value() > 1) {
        ui->spinPage->setValue(ui->spinPage->value() - 1);
        loadCurrentPage();
    } else {
        appendToOutput("Already on first page");
    }
}

void MainWindow::onNextPageClicked()
{
    int limit = ui->comboLimit->currentText().toInt();
    int maxPage = (currentTotal + limit - 1) / limit;
    if (ui->spinPage->value() < maxPage) {
        ui->spinPage->setValue(ui->spinPage->value() + 1);
        loadCurrentPage();
    } else {
        appendToOutput(QString("Already on last page (page %1 of %2)").arg(ui->spinPage->value()).arg(maxPage));
    }
}

void MainWindow::onRefreshClicked()
{
    appendToOutput("Refreshing current page...");
    loadCurrentPage();
}

void MainWindow::onPageChanged()
{
    loadCurrentPage();
}

void MainWindow::onLimitChanged()
{
    ui->spinPage->setValue(1);
    appendToOutput(QString("Items per page changed to: %1").arg(ui->comboLimit->currentText()));
    loadCurrentPage();
}

void MainWindow::onExitTriggered()
{
    appendToOutput("Exiting application...");
    close();
}

void MainWindow::onAboutTriggered()
{
    QMessageBox::about(this, "About Anime REST Client",
                       "<h3>Anime REST Client</h3>"
                       "<p>Version 1.0</p>"
                       "<p><b>Laboratory Work 3</b><br>REST API Client with Qt</p>"
                       "<p><b>Features:</b></p>"
                       "<ul>"
                       "<li>Generated OpenAPI client with callbacks (signals)</li>"
                       "<li>Asynchronous HTTP requests</li>"
                       "<li>Full CRUD operations support</li>"
                       "<li>Pagination support (10/20/50/100 items per page)</li>"
                       "<li>Detailed logging with all 13 fields</li>"
                       "</ul>"
                       "<p><b>Server URL:</b> https://localhost (via nginx)</p>");
}

void MainWindow::onConnectClicked()
{
    QString newUrl = ui->serverUrlInput->text().trimmed();

    if (newUrl.isEmpty()) {
        appendToOutput("ERROR: Please enter a valid server URL");
        QMessageBox::warning(this, "Input Error", "Please enter a valid server URL");
        return;
    }

    while (newUrl.endsWith('/')) {
        newUrl.chop(1);
    }

    appendToOutput(QString("Changing server URL to: %1").arg(newUrl));
    apiClient->setNewServerForAllOperations(QUrl(newUrl));
    loadCurrentPage();
}
