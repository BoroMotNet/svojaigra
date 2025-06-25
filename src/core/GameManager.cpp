#include "GameManager.h"
#include "FileManager.h"
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>
#include <QRandomGenerator>

GameManager &GameManager::instance() {
    static GameManager instance;
    return instance;
}

GameManager::GameManager()
    : QObject(nullptr),
      m_currentState(GameState::NotStarted),
      m_currentPlayerIndex(0),
      m_currentQuestion(nullptr) {
}

const std::vector<Player> &GameManager::getPlayers() const {
    return m_players;
}

int GameManager::getCurrentPlayerIndex() const {
    return m_currentPlayerIndex;
}

QStringList GameManager::getCategories() const {
    QStringList categories;
    for (const auto &pair: m_gameBoard) {
        categories << pair.first;
    }
    return categories;
}

const Question* GameManager::getQuestion(const QString &category, int points) const {
    auto it_cat = m_gameBoard.find(category);
    if (it_cat != m_gameBoard.end()) {
        auto it_q = it_cat->second.find(points);
        if (it_q != it_cat->second.end()) {
            return &it_q->second;
        }
    }
    return nullptr;
}

bool GameManager::isQuestionAnswered(const QString &category, int points) const {
    const Question* q = getQuestion(category, points);
    return !q || q->answered;
}

void GameManager::startGame(const QStringList &playerNames) {
    m_players.clear();
    for (const QString &name: playerNames) {
        m_players.emplace_back(name);
    }

    setupBoard();

    if (m_gameBoard.empty()) {
        qWarning() << tr("Не удалось загрузить вопросы. Игра не может быть начата.");
        QMessageBox::critical(nullptr, tr("Ошибка загрузки"),
                              tr("Не удалось загрузить вопросы. Проверьте файлы игры."));
        return;
    }

    m_currentPlayerIndex = 0;
    m_currentState = GameState::SelectingQuestion;

    emit gameBoardUpdated();
    emit currentPlayerChanged(m_currentPlayerIndex);
    emit gameStateChanged(m_currentState);
}

void GameManager::setupBoard() {
    m_gameBoard.clear();
    QMap<QString, std::vector<Question>> groupedQuestions = FileManager::loadGroupedQuestionsForGame(FileManager::loadLanguageSetting());

    for (auto it = groupedQuestions.constBegin(); it != groupedQuestions.constEnd(); ++it) {
        const QString& category = it.key();
        const std::vector<Question>& questions = it.value();
        for (const Question& q : questions) {
            m_gameBoard[category][q.points] = q;
        }
    }
}

bool GameManager::isQuestionSelectable(const QString &category, int points) const {
    if (isQuestionAnswered(category, points)) {
        return false;
    }
    const Player &currentPlayer = m_players.at(m_currentPlayerIndex);
    if (currentPlayer.shouldLockEasyQuestions()) {
        return points > 300;
    }
    return true;
}

void GameManager::selectQuestion(const QString &category, int points) {
    if (m_currentState != GameState::SelectingQuestion || !isQuestionSelectable(category, points)) return;
    m_currentQuestion = &m_gameBoard.at(category).at(points);
    m_currentState = GameState::AnsweringQuestion;
    m_answerTimer.start();
    emit questionSelected(*m_currentQuestion);
    emit gameStateChanged(m_currentState);
}

void GameManager::selectRandomQuestion() {
    if (m_currentState != GameState::SelectingQuestion) return;

    std::vector<Question*> availableQuestions;
    for (auto& categoryPair : m_gameBoard) {
        for (auto& questionPair : categoryPair.second) {
            if (isQuestionSelectable(categoryPair.first, questionPair.first)) {
                availableQuestions.push_back(&questionPair.second);
            }
        }
    }

    if (availableQuestions.empty()) {
        QMessageBox::information(nullptr, tr("Случайный вопрос"), tr("Нет доступных вопросов для выбора."));
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(static_cast<int>(availableQuestions.size()));
    Question* randomQuestion = availableQuestions[randomIndex];
    selectQuestion(randomQuestion->category, randomQuestion->points);
}

void GameManager::submitAnswer(const QString &answer) {
    if (m_currentState != GameState::AnsweringQuestion || !m_currentQuestion) return;

    double timeElapsed = m_answerTimer.elapsed() / 1000.0;
    bool isCorrect = m_currentQuestion->checkAnswer(answer);
    Player &currentPlayer = m_players[m_currentPlayerIndex];
    int answeringPlayerIndex = m_currentPlayerIndex;
    bool thresholdWasAlreadySet = currentPlayer.shouldLockEasyQuestions();
    currentPlayer.recordAnswer(isCorrect, timeElapsed);

    if (isCorrect) {
        currentPlayer.addScore(m_currentQuestion->points);
    }

    m_currentQuestion->answered = true;

    if (!thresholdWasAlreadySet && currentPlayer.shouldLockEasyQuestions()) {
        emit thresholdReached(currentPlayer.getName());
    }

    emit playerScoreUpdated(answeringPlayerIndex, currentPlayer.getScore());

    changeTurn();
    emit gameBoardUpdated();
    checkGameEnd();
}

std::vector<int> GameManager::getPointsForCategory(const QString &category) const {
    std::vector<int> points;
    auto it_cat = m_gameBoard.find(category);
    if (it_cat != m_gameBoard.end()) {
        for (const auto& pair : it_cat->second) {
            points.push_back(pair.first);
        }
        // Сортируем, чтобы очки всегда шли по порядку (100, 200, 300...)
        std::sort(points.begin(), points.end());
    }
    return points;
}

void GameManager::endGame() {
    m_currentState = GameState::GameFinished;
    FileManager::saveGameResults(m_players);
    emit gameFinished(m_players);
    emit gameStateChanged(m_currentState);
}

void GameManager::changeTurn() {
    m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
    m_currentState = GameState::SelectingQuestion;
    m_currentQuestion = nullptr;
    emit currentPlayerChanged(m_currentPlayerIndex);
    emit gameStateChanged(m_currentState);
}

void GameManager::checkGameEnd() {
    for (const auto &categoryPair : m_gameBoard) {
        for (const auto &questionPair : categoryPair.second) {
            if (!questionPair.second.answered) {
                return;
            }
        }
    }
    endGame();
}