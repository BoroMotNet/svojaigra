#include "GameManager.h"
#include "FileManager.h"
#include <QTimer>
#include <QDebug>
#include <algorithm>
#include <QMessageBox>
#include <QRandomGenerator>

// ... (код до startGame без изменений) ...
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

const std::vector<Player> &GameManager::getPlayers() const { return m_players; }
int GameManager::getCurrentPlayerIndex() const { return m_currentPlayerIndex; }

const std::map<int, Question *> &GameManager::getQuestionsForCategory(const QString &category) const {
    if (m_gameBoard.count(category)) {
        return m_gameBoard.at(category);
    }
    static const std::map<int, Question *> emptyMap;
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
    // В новой логике сыгранный вопрос - это nullptr в m_gameBoard
    if (!m_gameBoard.count(category) || !m_gameBoard.at(category).count(points)) {
        return true; // Вопроса нет, значит он "отвечен"
    }
    return m_gameBoard.at(category).at(points) == nullptr;
}

void GameManager::startGame(const QStringList &playerNames) {
    m_players.clear();
    for (const QString &name: playerNames) {
        m_players.emplace_back(name);
    }

    // ИЗМЕНЕНИЕ: Вызываем правильную функцию и она возвращает нужный тип std::vector<Question>
    m_allQuestions = FileManager::loadQuestionsForGame(FileManager::loadLanguageSetting());

    if (m_allQuestions.empty()) {
        qWarning() << tr("Не удалось загрузить вопросы. Игра не может быть начата.");
        // Здесь можно показать QMessageBox пользователю
        return;
    }

    setupBoard();
    m_currentPlayerIndex = 0;
    m_currentState = GameState::SelectingQuestion;

    emit gameBoardUpdated();
    emit currentPlayerChanged(m_currentPlayerIndex);
    emit gameStateChanged(m_currentState);
}

// ... (остальной код GameManager.cpp без изменений) ...
bool GameManager::isQuestionSelectable(const QString &category, int points) const {
    if (isQuestionAnswered(category, points)) {
        return false;
    }
    // const Player &currentPlayer = m_players.at(m_currentPlayerIndex);
    // return points >= currentPlayer.getPointsThreshold();
    return true; // Логику с порогом пока убрал для простоты
}

void GameManager::selectQuestion(const QString &category, int points) {
    if (m_currentState != GameState::SelectingQuestion) return;

    if (!isQuestionSelectable(category, points)) {
        // QMessageBox::warning(...);
        return;
    }

    m_currentQuestion = m_gameBoard.at(category).at(points);
    m_currentState = GameState::AnsweringQuestion;

    if (m_currentQuestion == nullptr) {
        qWarning() << tr("Попытка выбрать уже сыгранный вопрос!");
        return;
    }
    m_answerTimer.start();

    emit questionSelected(*m_currentQuestion);
    emit gameStateChanged(m_currentState);
}

void GameManager::selectRandomQuestion() {
    if (m_currentState != GameState::SelectingQuestion) {
        qWarning() << tr("Невозможно выбрать случайный вопрос: игра не в состоянии выбора вопроса.");
        return;
    }

    std::vector<std::pair<QString, int>> availableQuestions;

    for (const auto& categoryPair : m_gameBoard) {
        const QString& category = categoryPair.first;
        for (const auto& questionPair : categoryPair.second) {
            int points = questionPair.first;
            // Проверяем, что вопрос еще не сыгран (указатель не null)
            if (questionPair.second != nullptr && isQuestionSelectable(category, points)) {
                availableQuestions.emplace_back(category, points);
            }
        }
    }

    if (availableQuestions.empty()) {
        QMessageBox::information(nullptr, tr("Случайный вопрос"),
                                 tr("Нет доступных вопросов для выбора."));
        qInfo() << tr("Нет доступных вопросов для выбора случайного.");
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(int(availableQuestions.size()));
    QString randomCategory = availableQuestions[randomIndex].first;
    int randomPoints = availableQuestions[randomIndex].second;

    qInfo() << tr("Выбран случайный вопрос: Категория '") << randomCategory
            << tr("', Очки: ") << randomPoints;

    selectQuestion(randomCategory, randomPoints);
}


void GameManager::submitAnswer(const QString &answer) {
    if (m_currentState != GameState::AnsweringQuestion || !m_currentQuestion) return;
    double timeElapsed = m_answerTimer.elapsed() / 1000.0;

    // В вашей структуре Question нет метода checkAnswer, использую прямое сравнение
    bool isCorrect = (m_currentQuestion->answer.toLower() == answer.toLower());
    Player &currentPlayer = m_players[m_currentPlayerIndex];

    // currentPlayer.recordAnswer(isCorrect, timeElapsed); // Метода тоже нет

    if (isCorrect) {
        currentPlayer.addScore(m_currentQuestion->points);
    } else {
        // Логика для неправильного ответа (если нужна)
    }

    // Помечаем вопрос как сыгранный
    m_gameBoard.at(m_currentQuestion->category).at(m_currentQuestion->points) = nullptr;

    emit playerScoreUpdated(m_currentPlayerIndex, currentPlayer.getScore());
    emit gameBoardUpdated();

    changeTurn();
    checkGameEnd();
}

void GameManager::endGame() {
    m_currentState = GameState::GameFinished;
    FileManager::saveGameResults(m_players);
    emit gameFinished(m_players);
    emit gameStateChanged(m_currentState);
}


void GameManager::setupBoard() {
    m_gameBoard.clear();
    // m_allQuestions - это std::vector<Question>, владеющий объектами
    for (Question &q: m_allQuestions) {
        // m_gameBoard хранит указатели на эти объекты
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
                // Если найден хоть один несыгранный вопрос, выходим
                return;
            }
        }
    }
    // Если циклы завершились, значит, несыгранных вопросов нет
    endGame();
}