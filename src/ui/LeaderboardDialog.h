#ifndef LEADERBOARDDIALOG_H
#define LEADERBOARDDIALOG_H

#include <QDialog>

class QTableWidget; // Используем предварительное объявление

class LeaderboardDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LeaderboardDialog(QWidget *parent = nullptr);

private:
    void setupUi();
    void populateTable(); // Функция для заполнения таблицы данными

    QTableWidget* m_tableWidget;
};

#endif // LEADERBOARDDIALOG_H