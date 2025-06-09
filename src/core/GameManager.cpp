    /*
     * TODO: Убрать ступенчатое повышение сложности при стрике
     */

    #include "./GameManager.h"
    #include "./FileManager.h"
    #include <QTimer>
    #include <QDebug>
    #include <algorithm>

    // --- Реализация синглтона ---
    GameManager& GameManager::instance()
    {
        static GameManager instance; // Создается один раз при первом вызове
        return instance;
    }

    // --- Приватный конструктор ---
    GameManager::GameManager()
        : QObject(nullptr),
          m_currentState(GameState::NotStarted),
          m_currentPlayerIndex(0),
          m_currentQuestion(nullptr)
    {
    }

    // --- Методы для получения данных ---
    const std::vector<Player>& GameManager::getPlayers() const { return m_players; }
    int GameManager::getCurrentPlayerIndex() const { return m_currentPlayerIndex; }

const std::map<int, Question*>& GameManager::getQuestionsForCategory(const QString& category) const
    {
        return m_gameBoard.at(category);
    }

    QStringList GameManager::getCategories() const {
        QStringList categories;
        for(const auto& pair : m_gameBoard) {
            categories << pair.first;
        }
        return categories;
    }

    const Question* GameManager::getQuestion(const QString& category, int points) const {
        if (m_gameBoard.count(category) && m_gameBoard.at(category).count(points)) {
            return m_gameBoard.at(category).at(points);
        }
        return nullptr;
    }

    bool GameManager::isQuestionAnswered(const QString& category, int points) const {
        return getQuestion(category, points) == nullptr;
    }


    // --- Слоты управления игрой ---
    void GameManager::startGame(const QStringList& playerNames)
    {
        m_players.clear();
        for (const QString& name : playerNames) {
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

    bool GameManager::isQuestionSelectable(const QString& category, int points) const {
        if (isQuestionAnswered(category, points)) {
            return false;
        }
        const Player& currentPlayer = m_players.at(m_currentPlayerIndex);
        return points >= currentPlayer.getPointsThreshold();
    }

    void GameManager::selectQuestion(const QString& category, int points)
    {
        if (m_currentState != GameState::SelectingQuestion) return;

        if (!isQuestionSelectable(category, points)) {
            qWarning() << tr("Игрок") << m_players.at(m_currentPlayerIndex).getName()
                       << tr("не может выбрать этот вопрос (порог сложности)!");
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

    void GameManager::submitAnswer(const QString& answer)
    {
        if (m_currentState != GameState::AnsweringQuestion || !m_currentQuestion) return;
        double timeElapsed = m_answerTimer.elapsed() / 1000.0;

        bool isCorrect = m_currentQuestion->checkAnswer(answer);
        Player& currentPlayer = m_players[m_currentPlayerIndex];

        currentPlayer.recordAnswer(isCorrect, timeElapsed);

        if (isCorrect) {
            currentPlayer.addScore(m_currentQuestion->points);
            currentPlayer.recordAnswer(true, 0);

            if (currentPlayer.shouldLockEasyQuestions()) {
                int newThreshold = m_currentQuestion->points;
                if (newThreshold > currentPlayer.getPointsThreshold()) {
                    currentPlayer.setPointsThreshold(newThreshold);
                    qInfo() << tr("Порог для игрока") << currentPlayer.getName() << tr("повышен до") << newThreshold;
                }
                currentPlayer.clearAnswerHistory();
            }

        } else {
            currentPlayer.addScore(-m_currentQuestion->points);
            currentPlayer.clearAnswerHistory();
        }

        m_gameBoard.at(m_currentQuestion->category).at(m_currentQuestion->points) = nullptr;

        emit playerScoreUpdated(m_currentPlayerIndex, currentPlayer.getScore());
        emit gameBoardUpdated();

        changeTurn();
        checkGameEnd();
    }

    void GameManager::endGame()
    {
        m_currentState = GameState::GameFinished;
        FileManager::saveGameResults(m_players);
        emit gameFinished(m_players);
        emit gameStateChanged(m_currentState);
    }


    // --- Приватные методы ---
    void GameManager::setupBoard()
    {
        m_gameBoard.clear();
        for (Question& q : m_allQuestions) {
            m_gameBoard[q.category][q.points] = &q;
        }
    }

    void GameManager::changeTurn()
    {
        m_currentPlayerIndex = (m_currentPlayerIndex + 1) % m_players.size();
        m_currentState = GameState::SelectingQuestion;
        m_currentQuestion = nullptr;

        emit currentPlayerChanged(m_currentPlayerIndex);
        emit gameStateChanged(m_currentState);
    }

    void GameManager::checkGameEnd()
    {
        for (const auto& categoryPair : m_gameBoard) {
            for (const auto& questionPair : categoryPair.second) {
                if (questionPair.second != nullptr) {
                    // Найден хотя бы один несыгранный вопрос, игра продолжается
                    return;
                }
            }
        }
        // Если циклы завершились, несыгранных вопросов нет
        endGame();
    }