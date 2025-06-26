#include "MainWindow.h"
#include "QuestionDialog.h"
#include "../core/GameManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>
#include <QKeyEvent>
#include <QApplication>

// КОНСТРУКТОР: Больше не принимает playerNames. Все данные берутся из GameManager.
MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi(); // Этот метод вызывает всё остальное
    applyCurrentSettings();

    // Соединяем сигналы от логики к слотам нашего интерфейса
    connect(&GameManager::instance(), &GameManager::gameBoardUpdated, this, &MainWindow::updateBoardState);
    connect(&GameManager::instance(), &GameManager::currentPlayerChanged, this, &MainWindow::updatePlayerInfo);
    connect(&GameManager::instance(), &GameManager::playerScoreUpdated, this, &MainWindow::updatePlayerScore);
    connect(&GameManager::instance(), &GameManager::questionSelected, this, &MainWindow::displayQuestion);
    connect(&GameManager::instance(), &GameManager::gameFinished, this, &MainWindow::showGameResults);

    // Устанавливаем начальное состояние интерфейса
    updatePlayerInfo(GameManager::instance().getCurrentPlayerIndex());
}

// СОБЫТИЕ ЗАКРЫТИЯ: Твоя логика, она корректна.
void MainWindow::closeEvent(QCloseEvent *event) {
    // Показываем родительское окно (наше главное меню)
    if (parentWidget()) {
        parentWidget()->show();
    }
    // Принимаем событие, чтобы окно действительно закрылось
    event->accept();
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        // Вызываем close(), который, в свою очередь, вызовет наш closeEvent
        // перед тем, как предложить завершить игру.
        handleExitClicked();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::setupUi() {
    setWindowTitle(tr("Своя Игра"));
    resize(1280, 720);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_playerLayout = new QHBoxLayout();
    m_boardLayout = new QGridLayout();

    QHBoxLayout *controlButtonLayout = new QHBoxLayout();
    controlButtonLayout->setSpacing(10);

    m_randomQuestionButton = new QPushButton(tr("Случайный вопрос"), this);
    m_exitButton = new QPushButton(tr("Выход"), this);

    QFont buttonFont("Arial", 12, QFont::Bold);
    m_randomQuestionButton->setFont(buttonFont);
    m_exitButton->setFont(buttonFont);

    controlButtonLayout->addWidget(m_randomQuestionButton);
    controlButtonLayout->addStretch(1);
    controlButtonLayout->addWidget(m_exitButton);

    m_boardLayout->setSpacing(10);
    m_mainLayout->setSpacing(20);

    m_mainLayout->addLayout(m_playerLayout, 1);
    m_mainLayout->addLayout(m_boardLayout, 8);
    m_mainLayout->addLayout(controlButtonLayout, 1);

    connect(m_randomQuestionButton, &QPushButton::clicked, this, &MainWindow::handleRandomQuestionClicked);
    connect(m_exitButton, &QPushButton::clicked, this, &MainWindow::handleExitClicked);

    // Настраиваем доску и игроков
    setupPlayerBar();
    setupGameBoard();
}

// SETUPPLAYERBAR: Убрали лишний аргумент. Логика осталась твоей.
void MainWindow::setupPlayerBar() {
    QLayoutItem *item;
    while ((item = m_playerLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    m_playerNameLabels.clear();
    m_playerScoreLabels.clear();

    const auto& players = GameManager::instance().getPlayers();
    for (const auto &player : players) {
        QVBoxLayout *playerBox = new QVBoxLayout();

        QLabel *nameLabel = new QLabel(player.getName(), this);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setFont(QFont("Arial", 16, QFont::Bold));
        m_playerNameLabels.push_back(nameLabel);

        // Используем getScore(), чтобы корректно отобразить счет при пересоздании окна
        QLabel *scoreLabel = new QLabel(QString::number(player.getScore()), this);
        scoreLabel->setAlignment(Qt::AlignCenter);
        scoreLabel->setFont(QFont("Arial", 20, QFont::Bold));
        scoreLabel->setStyleSheet("color: yellow;");
        m_playerScoreLabels.push_back(scoreLabel);

        playerBox->addWidget(nameLabel);
        playerBox->addWidget(scoreLabel);
        m_playerLayout->addLayout(playerBox);
    }
}

// SETUPGAMEBOARD: Твоя версия, она корректна.
void MainWindow::setupGameBoard() {
    QLayoutItem *item;
    while ((item = m_boardLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    m_questionButtons.clear();

    QStringList categories = GameManager::instance().getCategories();
    int column = 0;
    for (const QString &category: categories) {
        QLabel *categoryLabel = new QLabel(category, this);
        categoryLabel->setAlignment(Qt::AlignCenter);
        categoryLabel->setFont(QFont("Arial", 14, QFont::Bold));
        categoryLabel->setWordWrap(true);
        m_boardLayout->addWidget(categoryLabel, 0, column);

        std::vector<int> pointsList = GameManager::instance().getPointsForCategory(category);
        int row = 1;
        for (int points: pointsList) {
            QPushButton *questionButton = new QPushButton(QString::number(points), this);
            questionButton->setFont(QFont("Arial", 16, QFont::Bold));
            questionButton->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            connect(questionButton, &QPushButton::clicked, this, [category, points]() {
                GameManager::instance().selectQuestion(category, points);
            });

            m_boardLayout->addWidget(questionButton, row, column);
            m_questionButtons[category][points] = questionButton;
            row++;
        }
        column++;
    }
    updateBoardState(); // Сразу обновляем состояние кнопок
}

// UPDATEBOARDSTATE: Твоя версия, она корректна.
void MainWindow::updateBoardState() {
    for (auto it_cat = m_questionButtons.constBegin(); it_cat != m_questionButtons.constEnd(); ++it_cat) {
        const QString &category = it_cat.key();
        const QMap<int, QPushButton *> &pointsMap = it_cat.value();
        for (auto it_q = pointsMap.constBegin(); it_q != pointsMap.constEnd(); ++it_q) {
            int points = it_q.key();
            QPushButton *button = it_q.value();

            if (GameManager::instance().isQuestionAnswered(category, points)) {
                button->setEnabled(false);
                button->setText("✓");
            } else {
                button->setText(QString::number(points));
                button->setEnabled(GameManager::instance().isQuestionSelectable(category, points));
            }
        }
    }
}

// ... (методы updatePlayerInfo, updatePlayerScore, displayQuestion остаются без изменений) ...

void MainWindow::updatePlayerInfo(int playerIndex) {
    for (int i = 0; i < m_playerNameLabels.size(); ++i) {
        if (i == playerIndex) {
            m_playerNameLabels[i]->setStyleSheet("color: lightgreen; border: 2px solid lightgreen; padding: 2px;");
        } else {
            m_playerNameLabels[i]->setStyleSheet("color: white; border: none;");
        }
    }
}

void MainWindow::updatePlayerScore(int playerIndex, int newScore) {
    if (playerIndex >= 0 && playerIndex < m_playerScoreLabels.size()) {
        m_playerScoreLabels[playerIndex]->setText(QString::number(newScore));
    }
}

void MainWindow::displayQuestion(const Question &question) {
    QuestionDialog dialog(question, this);
    if (dialog.exec() == QDialog::Accepted) {
        GameManager::instance().submitAnswer(dialog.getAnswer());
    } else {
        GameManager::instance().submitAnswer(""); // Закрытие диалога = неверный ответ
    }
}

// SHOWGAMERESULTS: Твоя версия, она корректна.
void MainWindow::showGameResults(const std::vector<Player> &finalResults) {
    QString resultsText;
    for (const auto &player: finalResults) {
        resultsText += QString("%1: %2\n").arg(player.getName()).arg(player.getScore());
    }
    QMessageBox::information(this, tr("Игра окончена!"), resultsText);
    close();
}

void MainWindow::applyCurrentSettings() {
    QSettings settings;
    QString colorName = settings.value("backgroundColor", "#333333").toString();
    QString style = QString("QWidget#centralWidget { background-color: %1; }").arg(colorName);
    m_centralWidget->setObjectName("centralWidget");
    this->setStyleSheet(style);
}

void MainWindow::handleRandomQuestionClicked() {
    GameManager::instance().selectRandomQuestion();
}

// HANDLEEXITCLICKED: Твоя версия, она корректна.
void MainWindow::handleExitClicked() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, tr("Завершение игры"),
                                  tr("Вы уверены, что хотите завершить игру досрочно?"),
                                  QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        // Завершаем игру. GameManager пошлет сигнал gameFinished,
        // который вызовет showGameResults, а тот закроет окно.
        GameManager::instance().endGame();
    }
}