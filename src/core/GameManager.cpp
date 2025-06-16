#include "./GameManager.h"
#include "./FileManager.h"
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>
#include <QRandomGenerator>

// --- Реализация синглтона ---
GameManager &GameManager::instance() {
    static GameManager instance; // Создается один раз при первом вызове
    return instance;
}

// --- Приватный конструктор ---
GameManager::GameManager()
    : QObject(nullptr),
      m_currentState(GameState::NotStarted),
      m_currentPlayerIndex(0),
      m_currentQuestion(nullptr) {
}

// --- Методы для получения данных ---
const std::vector<Player> &GameManager::getPlayers() const { return m_players; }
int GameManager::getCurrentPlayerIndex() const { return m_currentPlayerIndex; }

const std::map<int, Question *> &GameManager::getQuestionsForCategory(const QString &category) const {
    // Ensure the category exists before accessing
    if (m_gameBoard.count(category)) {
        return m_gameBoard.at(category);
    }
    // Return an empty map if category not found to prevent crash
    static const std::map<int, Question *> emptyMap; // Use a static empty map to return a const reference
    return emptyMap;
}

QStringList GameManager::getCategories() const {
    QStringList categories;
    for (const auto &pair: m_gameBoard) {
        categories << pair.first;
    }
    return categories;
}

const Question *GameManager::getQuestion(const QString &category, int points) const {
    if (m_gameBoard.count(category) && m_gameBoard.at(category).count(points)) {
        return m_gameBoard.at(category).at(points);
    }
    return nullptr;
}

bool GameManager::isQuestionAnswered(const QString &category, int points) const {
    return getQuestion(category, points) == nullptr;
}


// --- Слоты управления игрой ---
void GameManager::startGame(const QStringList &playerNames) {
    m_players.clear();
    for (const QString &name: playerNames) {
        m_players.emplace_back(name);
    }

    m_allQuestions = FileManager::loadQuestions(FileManager::loadLanguageSetting());
    if (m_allQuestions.empty()) {
        qWarning() << tr("Не удалось загрузить вопросы. Игра не может быть начата.");
        return;
    }

    setupBoard();
    m_currentPlayerIndex = 0;
    m_currentState = GameState::SelectingQuestion;

    emit gameBoardUpdated();
    emit currentPlayerChanged(m_currentPlayerIndex);
    emit gameStateChanged(m_currentState);
}

bool GameManager::isQuestionSelectable(const QString &category, int points) const {
    if (isQuestionAnswered(category, points)) {
        return false;
    }
    const Player &currentPlayer = m_players.at(m_currentPlayerIndex);
    // Question is selectable if its points are greater than or equal to the player's threshold
    return points >= currentPlayer.getPointsThreshold();
}

void GameManager::selectQuestion(const QString &category, int points) {
    if (m_currentState != GameState::SelectingQuestion) return;

    // Check if the question is selectable based on the current player's threshold
    if (!isQuestionSelectable(category, points)) {
        QMessageBox::warning(nullptr, tr("Недоступный вопрос"),
                             tr("Игрок %1 не может выбрать этот вопрос из-за порога сложности (%2 очков).")
                             .arg(m_players.at(m_currentPlayerIndex).getName())
                             .arg(m_players.at(m_currentPlayerIndex).getPointsThreshold()));
        return;
    }

    m_currentQuestion = m_gameBoard.at(category).at(points);
    m_currentState = GameState::AnsweringQuestion;

    if (m_currentQuestion == nullptr) {
        qWarning() << tr("Попытка выбрать уже сыгранный вопрос!");
        return;
    }
    m_answerTimer.start(); // Запускаем отсчет времени на ответ

    emit questionSelected(*m_currentQuestion);
    emit gameStateChanged(m_currentState);
}

void GameManager::selectRandomQuestion() {
    if (m_currentState != GameState::SelectingQuestion) {
        qWarning() << tr("Невозможно выбрать случайный вопрос: игра не в состоянии выбора вопроса.");
        return;
    }

    std::vector<std::pair<QString, int>> availableQuestions; // Store category and points of selectable questions

    // Iterate through all categories and questions
    for (const auto& categoryPair : m_gameBoard) {
        const QString& category = categoryPair.first;
        for (const auto& questionPair : categoryPair.second) {
            int points = questionPair.first;
            // Check if the question is selectable for the current player
            if (isQuestionSelectable(category, points)) {
                availableQuestions.emplace_back(category, points);
            }
        }
    }

    if (availableQuestions.empty()) {
        QMessageBox::information(nullptr, tr("Случайный вопрос"),
                                 tr("Нет доступных вопросов для текущего игрока, учитывая порог сложности."));
        qInfo() << tr("Нет доступных вопросов для выбора случайного.");
        return;
    }

    // Pick a random question from the available ones
    int randomIndex = QRandomGenerator::global()->bounded(availableQuestions.size());
    QString randomCategory = availableQuestions[randomIndex].first;
    int randomPoints = availableQuestions[randomIndex].second;

    qInfo() << tr("Выбран случайный вопрос: Категория '") << randomCategory
            << tr("', Очки: ") << randomPoints;

    // Now, call the existing selectQuestion method with the randomly chosen question
    selectQuestion(randomCategory, randomPoints);
}


void GameManager::submitAnswer(const QString &answer) {
    if (m_currentState != GameState::AnsweringQuestion || !m_currentQuestion) return;
    double timeElapsed = m_answerTimer.elapsed() / 1000.0;

    bool isCorrect = m_currentQuestion->checkAnswer(answer);
    Player &currentPlayer = m_players[m_currentPlayerIndex]; // Get mutable reference to current player

    // Record the answer. The Player class itself will handle history and clearing on incorrect.
    currentPlayer.recordAnswer(isCorrect, timeElapsed);

    if (isCorrect) {
        currentPlayer.addScore(m_currentQuestion->points);

        if (!currentPlayer.hasThresholdBeenSet() && currentPlayer.shouldLockEasyQuestions()) {
            int newThreshold = m_currentQuestion->points;
            currentPlayer.setPointsThreshold(newThreshold);
            currentPlayer.setThresholdSet(true); // Mark threshold as set for this player

            qInfo() << tr("Порог сложности для игрока") << currentPlayer.getName()
                    << tr("установлен на") << newThreshold << tr("очков.");

            // Clear answer history *after* setting the threshold to prevent
            // re-triggering this logic on subsequent quick answers.
            currentPlayer.clearAnswerHistory();
        }
    } else {
        // --- ИЗМЕНЕНИЕ ЗДЕСЬ ---
        // Если ответ неправильный, просто ничего не делаем с баллами.
        // currentPlayer.addScore(-m_currentQuestion->points); // Эту строку мы УДАЛЯЕМ
        // Player::recordAnswer уже очистит историю, так что здесь ничего больше не нужно.
    }

    // Mark the question as answered (set to nullptr)
    m_gameBoard.at(m_currentQuestion->category).at(m_currentQuestion->points) = nullptr;

    emit playerScoreUpdated(m_currentPlayerIndex, currentPlayer.getScore());
    emit gameBoardUpdated(); // Update board state to gray out the answered question

    changeTurn();
    checkGameEnd();
}

void GameManager::endGame() {
    m_currentState = GameState::GameFinished;
    FileManager::saveGameResults(m_players);
    emit gameFinished(m_players);
    emit gameStateChanged(m_currentState);
}


// --- Приватные методы ---
void GameManager::setupBoard() {
    m_gameBoard.clear();
    for (Question &q: m_allQuestions) {
        m_gameBoard[q.category][q.points] = &q;
    }
}

void GameManager::changeTurn() {
    m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
    m_currentState = GameState::SelectingQuestion;
    m_currentQuestion = nullptr;

    emit currentPlayerChanged(m_currentPlayerIndex);
    emit gameStateChanged(m_currentState);
}

void GameManager::checkGameEnd() {
    for (const auto &categoryPair: m_gameBoard) {
        for (const auto &questionPair: categoryPair.second) {
            if (questionPair.second != nullptr) {
                return;
            }
        }
    }
    endGame();
}