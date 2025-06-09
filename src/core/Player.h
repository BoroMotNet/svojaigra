#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <vector>
#include <utility>

/**
 * @class Player
 * @brief Класс для хранения данных и состояния одного игрока.
 *
 * Отвечает за имя игрока, его текущий счет, а также хранит историю
 * последних ответов для реализации механики динамической сложности.
 */
class Player
{
public:
    /**
     * @brief Конструктор класса Player.
     * @param name Имя игрока.
     */
    explicit Player(const QString &name);

    /**
     * @brief Возвращает имя игрока.
     * @return QString с именем игрока.
     */
    QString getName() const;

    /**
     * @brief Возвращает текущий счет игрока.
     * @return int со счетом игрока.
     */
    int getScore() const;

    /**
     * @brief Добавляет очки к счету игрока.
     * @param points Количество очков (может быть отрицательным).
     */
    void addScore(int points);

    /**
     * @brief Сбрасывает счет игрока до нуля.
     * Полезно при начале новой игры.
     */
    void resetScore();

    /**
     * @brief Записывает результат ответа игрока.
     * Добавляет в историю пару (правильность, время).
     * История ограничена константой HISTORY_SIZE_FOR_DIFFICULTY_CHECK.
     * @param isCorrect true, если ответ был правильным, иначе false.
     * @param timeInSeconds Время, затраченное на ответ, в секундах.
     */
    void recordAnswer(bool isCorrect, double timeInSeconds);

    /**
     * @brief Проверяет, нужно ли усложнять вопросы для этого игрока.
     * Анализирует историю последних ответов.
     * @return true, если последние N ответов были правильными и быстрыми, иначе false.
     */
    bool shouldIncreaseDifficulty() const;


private:
    // --- Константы для логики усложнения ---

    /// Количество последних ответов, которые анализируются для усложнения.
    static const int HISTORY_SIZE_FOR_DIFFICULTY_CHECK = 2;
    /// Пороговое значение времени в секундах. Ответ считается быстрым, если он дан быстрее этого времени.
    static constexpr double QUICK_ANSWER_THRESHOLD_S = 5.0;


    // --- Поля класса ---

    /// Имя игрока.
    QString m_name;

    /// Текущий счет игрока.
    int m_score;

    /// История последних ответов. Хранит пары: <правильность ответа, время ответа>.
    /// Используется для определения необходимости усложнения вопросов.
    std::vector<std::pair<bool, double>> m_answerHistory;

};

#endif // PLAYER_H