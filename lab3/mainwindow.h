#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTableWidgetItem>
#include "OAIDefaultApi.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onListClicked();
    void onGetClicked();
    void onCreateClicked();
    void onUpdateClicked();
    void onDeleteClicked();
    void onExitTriggered();
    void onAboutTriggered();
    void onConnectClicked();
    void onPrevPageClicked();
    void onNextPageClicked();
    void onRefreshClicked();
    void onPageChanged();
    void onLimitChanged();

private:
    void appendToOutput(const QString& text);
    void setupConnections();
    void loadCurrentPage();

    Ui::MainWindow *ui;
    OpenAPI::OAIDefaultApi* apiClient;
    int currentTotal;
};

#endif // MAINWINDOW_H
