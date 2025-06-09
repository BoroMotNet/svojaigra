#include "./MainWindow.h"
#include "./QuestionDialog.h"
#include "../core/GameManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QMessageBox>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setupUi();
    applyCurrentSettings();

    connect(&GameManager::instance(), &GameManager::gameBoardUpdated, this, &MainWindow::updateBoardState);
    connect(&GameManager::instance(), &GameManager::currentPlayerChanged, this, &MainWindow::updatePlayerInfo);
    connect(&GameManager::instance(), &GameManager::playerScoreUpdated, this, &MainWindow::updatePlayerScore);
    connect(&GameManager::instance(), &GameManager::questionSelected, this, &MainWindow::displayQuestion);
    connect(&GameManager::instance(), &GameManager::gameFinished, this, &MainWindow::showGameResults);

    updateBoardState();
    updatePlayerInfo(GameManager::instance().getCurrentPlayerIndex());
}

void MainWindow::setupUi()
{
    setWindowTitle(tr("Своя Игра"));
    resize(1280, 720);

    m_centralWidget = new QWidget(this);
    setCentralWidget(m_centralWidget);

    m_mainLayout = new QVBoxLayout(m_centralWidget);
    m_playerLayout = new QHBoxLayout();
    m_boardLayout = new QGridLayout();

    m_boardLayout->setSpacing(10);
    m_mainLayout->setSpacing(20);

    m_mainLayout->addLayout(m_playerLayout, 1);
    m_mainLayout->addLayout(m_boardLayout, 9);

    setupPlayerBar();
    setupGameBoard();
}

void MainWindow::setupPlayerBar()
{
    const auto& players = GameManager::instance().getPlayers();
    for (const auto& player : players) {
        QVBoxLayout* playerBox = new QVBoxLayout();

        QLabel* nameLabel = new QLabel(player.getName(), this);
        nameLabel->setAlignment(Qt::AlignCenter);
        nameLabel->setFont(QFont("Arial", 16, QFont::Bold));
        m_playerNameLabels.push_back(nameLabel);

        QLabel* scoreLabel = new QLabel(QString::number(player.getScore()), this);
        scoreLabel->setAlignment(Qt::AlignCenter);
        scoreLabel->setFont(QFont("Arial", 20, QFont::Bold));
        scoreLabel->setStyleSheet("color: yellow;");
        m_playerScoreLabels.push_back(scoreLabel);

        playerBox->addWidget(nameLabel);
        playerBox->addWidget(scoreLabel);
        m_playerLayout->addLayout(playerBox);
    }
}

void MainWindow::setupGameBoard()
{
    QStringList categories = GameManager::instance().getCategories();

    for (int col = 0; col < categories.size(); ++col) {
        QLabel* categoryLabel = new QLabel(categories[col], this);
        categoryLabel->setAlignment(Qt::AlignCenter);
        categoryLabel->setFont(QFont("Arial", 14, QFont::Bold));
        categoryLabel->setWordWrap(true);
        m_boardLayout->addWidget(categoryLabel, 0, col);
    }

    int pointsRow = 0;
    std::map<int, int> pointsToRowMap;

    for (const auto& category : categories) {
        const auto& pointsMap = GameManager::instance().getQuestionsForCategory(category);
        for(const auto& pair : pointsMap) {
            int points = pair.first;
            if(pointsToRowMap.find(points) == pointsToRowMap.end()) {
                pointsToRowMap[points] = ++pointsRow;
            }
        }
    }

    for (int col = 0; col < categories.size(); ++col) {
        const auto& category = categories[col];
        const auto& pointsMap = GameManager::instance().getQuestionsForCategory(category);

        for (const auto& pair : pointsMap) {
            int points = pair.first;
            int row = pointsToRowMap[points];

            QPushButton* button = new QPushButton(QString::number(points), this);
            button->setFont(QFont("Arial", 24, QFont::Bold));
            button->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

            button->setProperty("category", category);
            button->setProperty("points", points);

            connect(button, &QPushButton::clicked, this, &MainWindow::handleQuestionClicked);

            m_questionButtons[category][points] = button;
            m_boardLayout->addWidget(button, row, col);
        }
    }
}

void MainWindow::applyCurrentSettings()
{
    QSettings settings;
    QString colorName = settings.value("backgroundColor", "#333333").toString();
    QString style = QString("QWidget#centralWidget { background-color: %1; } QLabel { color: white; }").arg(colorName);
    m_centralWidget->setObjectName("centralWidget");
    this->setStyleSheet(style);

    int volume = settings.value("volume", 100).toInt();
}

void MainWindow::updateBoardState()
{
    for (auto const& [category, pointsMap] : m_questionButtons) {
        for (auto const& [points, button] : pointsMap) {
            if (GameManager::instance().isQuestionAnswered(category, points)) {
                button->setVisible(false);
            } else {
                button->setVisible(true);
                button->setEnabled(GameManager::instance().isQuestionSelectable(category, points));
            }
        }
    }
}

void MainWindow::updatePlayerInfo(int playerIndex)
{
    for (int i = 0; i < m_playerNameLabels.size(); ++i) {
        if (i == playerIndex) {
            m_playerNameLabels[i]->setStyleSheet("color: lightgreen; border: 2px solid lightgreen; padding: 2px;");
        } else {
            m_playerNameLabels[i]->setStyleSheet("color: white; border: none;");
        }
    }
    m_playerScoreLabels[playerIndex]->setStyleSheet("color: yellow;");
}

void MainWindow::updatePlayerScore(int playerIndex, int newScore)
{
    if (playerIndex < m_playerScoreLabels.size()) {
        m_playerScoreLabels[playerIndex]->setText(QString::number(newScore));
    }
}

void MainWindow::displayQuestion(const Question& question)
{
    QuestionDialog dialog(question, this);
    if (dialog.exec() == QDialog::Accepted) {
        GameManager::instance().submitAnswer(dialog.getAnswer());
    } else {
        GameManager::instance().submitAnswer("");
    }
}

void MainWindow::handleQuestionClicked()
{
    QPushButton* button = qobject_cast<QPushButton*>(sender());
    if (!button) return;

    QString category = button->property("category").toString();
    int points = button->property("points").toInt();

    GameManager::instance().selectQuestion(category, points);
}

void MainWindow::showGameResults(const std::vector<Player>& finalResults)
{
    QString resultsText;
    for(const auto& player : finalResults) {
        resultsText += QString("%1: %2\n").arg(player.getName()).arg(player.getScore());
    }
    QMessageBox::information(this, tr("Игра окончена!"), resultsText);
    close();
}