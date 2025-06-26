#include "LeaderboardDialog.h"
#include "../core/UserManager.h"

#include <QVBoxLayout>
#include <QTableWidget>
#include <QHeaderView>
#include <QPushButton>

LeaderboardDialog::LeaderboardDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    populateTable();
    setWindowTitle(tr("Таблица лидеров"));
    setMinimumSize(400, 500); // Задаем минимальный размер окна
}

void LeaderboardDialog::setupUi()
{
    auto* layout = new QVBoxLayout(this);
    m_tableWidget = new QTableWidget(this);
    m_tableWidget->setColumnCount(3); // 3 колонки: Место, Игрок, Очки
    m_tableWidget->setHorizontalHeaderLabels({tr("Место"), tr("Игрок"), tr("Очки")});

    // Настраиваем внешний вид таблицы
    m_tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers); // Запрет редактирования
    m_tableWidget->setSelectionBehavior(QAbstractItemView::SelectRows); // Выделение всей строки
    m_tableWidget->verticalHeader()->setVisible(false); // Скрываем номера строк
    m_tableWidget->horizontalHeader()->setStretchLastSection(true); // Последняя колонка растягивается
    m_tableWidget->setColumnWidth(0, 60); // Фиксированная ширина для колонки "Место"

    auto* closeButton = new QPushButton(tr("Закрыть"), this);
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);

    layout->addWidget(m_tableWidget);
    layout->addWidget(closeButton);
}

void LeaderboardDialog::populateTable()
{
    // Получаем отсортированный список пользователей
    QList<UserScore> scores = UserManager::instance().getAllUserScores();
    m_tableWidget->setRowCount(scores.size());

    int rank = 1;
    for (int i = 0; i < scores.size(); ++i) {
        const auto& userScore = scores.at(i);

        auto* rankItem = new QTableWidgetItem(QString::number(rank));
        auto* nameItem = new QTableWidgetItem(userScore.username);
        auto* scoreItem = new QTableWidgetItem(QString::number(userScore.totalScore));
        
        // Выравниваем текст по центру для наглядности
        rankItem->setTextAlignment(Qt::AlignCenter);
        scoreItem->setTextAlignment(Qt::AlignCenter);

        m_tableWidget->setItem(i, 0, rankItem);
        m_tableWidget->setItem(i, 1, nameItem);
        m_tableWidget->setItem(i, 2, scoreItem);
        
        rank++;
    }
}