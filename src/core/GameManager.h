#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <vector>
#include <QStringList>
#include <map>
#include "./Player.h"
#include "./Question.h"
#include <QElapsedTimer>


/**
 * @enum GameState
 * @brief Определяет текущее состояние игрового процесса.
 */
enum class GameState {
    NotStarted,         // Игра еще не началась
    PlayerSelection,    // Этап ввода имен игроков
    SelectingQuestion,  // Этап выбора вопроса активным игроком
    AnsweringQuestion,  // Этап ответа на вопрос
    GameFinished        // Игра окончена, показ результатов
};

/**
 * @class GameManager
 * @brief Центральный класс-синглтон, управляющий всей логикой игры.
 */
class GameManager : public QObject
{
    Q_OBJECT

public:
    /**
     * @brief Метод доступа к единственному экземпляру класса.
     * @return Ссылка на экземпляр GameManager.
     */
    static GameManager& instance();

    // --- Методы для получения данных (для UI) ---
    const std::vector<Player>& getPlayers() const;
    int getCurrentPlayerIndex() const;
    QStringList getCategories() const;
    const Question* getQuestion(const QString& category, int points) const;
    bool isQuestionAnswered(const QString& category, int points) const;

public slots:
    // --- Слоты для управления игрой (вызываются из UI) ---

    /**
     * @brief Начинает новую игру с указанными именами игроков.
     * @param playerNames Список имен игроков.
     */
    void startGame(const QStringList& playerNames);

    /**
     * @brief Выбирает вопрос на поле.
     * @param category Категория вопроса.
     * @param points Стоимость вопроса.
     */
    void selectQuestion(const QString& category, int points);

    /**
     * @brief Обрабатывает ответ, данный игроком.
     * @param answer Текст ответа.
     */
    void submitAnswer(const QString& answer);

    /**
     * @brief Завершает игру и сохраняет результаты.
     */
    void endGame();


signals:
    // --- Сигналы для уведомления UI об изменениях ---
    void gameStateChanged(GameState newState);
    void playerScoreUpdated(int playerIndex, int newScore);
    void currentPlayerChanged(int playerIndex);
    void questionSelected(const Question& question);
    void gameBoardUpdated(); // Сигнал для обновления всего игрового поля
    void gameFinished(const std::vector<Player>& finalResults);


private:
    // --- Приватные методы и конструкторы для синглтона ---
    GameManager(); // Конструктор приватный
    ~GameManager() = default;
    GameManager(const GameManager&) = delete; // Запрещаем копирование
    GameManager& operator=(const GameManager&) = delete;

    void setupBoard();
    void changeTurn();
    void checkGameEnd();


    // --- Поля состояния игры ---
    GameState m_currentState;
    std::vector<Question> m_allQuestions; // Все вопросы, загруженные из файла
    std::vector<Player> m_players;
    int m_currentPlayerIndex;

    // Указатель на текущий активный вопрос
    Question* m_currentQuestion;

    // Игровое поле: Категория -> (Очки -> Вопрос)
    // Используем указатели, чтобы можно было "удалять" сыгранные вопросы (ставить в nullptr)
    std::map<QString, std::map<int, Question*>> m_gameBoard;

    // Таймер для отслеживания времени ответа
    QElapsedTimer m_answerTimer;
};

#endif // GAMEMANAGER_H