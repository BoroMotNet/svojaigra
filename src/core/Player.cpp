#include "Player.h" // Включаем собственный заголовочный файл

/**
 * @brief Конструктор класса Player.
 * Инициализирует игрока с заданным именем и нулевым счетом.
 */
Player::Player(const QString &name)
    : m_name(name), // Инициализируем имя через список инициализации
      m_score(0),
      m_pointsThreshold(0),
      m_thresholdSet(false) // Initialize the new flag to false
{
}

/**
 * @brief Возвращает имя игрока.
 */
QString Player::getName() const {
    return m_name;
}

/**
 * @brief Возвращает текущий счет игрока.
 */
int Player::getScore() const {
    return m_score;
}

/**
 * @brief Добавляет очки к счету игрока.
 */
void Player::addScore(int points) {
    m_score += points;
}

/**
 * @brief Сбрасывает счет игрока до нуля.
 * Включает сброс порога сложности и истории ответов.
 */
void Player::resetScore() {
    m_score = 0;
    m_answerHistory.clear(); // Clear answer history
    m_pointsThreshold = 0;   // Reset points threshold
    m_thresholdSet = false;  // Reset threshold set flag
}

/**
 * @brief Записывает результат ответа игрока в его историю.
 * Если история становится слишком длинной, самый старый результат удаляется.
 * Если ответ был неверным, история очищается для сброса "серии".
 */
void Player::recordAnswer(bool isCorrect, double timeInSeconds) {
    // If the answer is incorrect, clear the entire history to break any streak
    if (!isCorrect) {
        clearAnswerHistory();
        return; // No need to add incorrect answer to history for streak check
    }

    // Add the new correct result to the end of the vector
    m_answerHistory.emplace_back(isCorrect, timeInSeconds);

    // If the history size exceeds the limit, remove the oldest element.
    if (m_answerHistory.size() > HISTORY_SIZE_FOR_DIFFICULTY_CHECK) {
        m_answerHistory.erase(m_answerHistory.begin());
    }
}

void Player::setPointsThreshold(int threshold)
{
    m_pointsThreshold = threshold;
}

int Player::getPointsThreshold() const
{
    return m_pointsThreshold;
}

void Player::clearAnswerHistory()
{
    m_answerHistory.clear();
}

/**
 * @brief Проверяет, нужно ли усложнять вопросы для этого игрока.
 * Возвращает true, если игрок имеет достаточно быстрых и правильных ответов
 * для установки порога сложности.
 */
bool Player::shouldLockEasyQuestions() const {
    // If the threshold has already been set, we don't need to check again.
    // This ensures the threshold is set only once.
    if (m_thresholdSet) {
        return false;
    }

    // 1. Check if we have enough data (correct answers) for analysis.
    if (m_answerHistory.size() < HISTORY_SIZE_FOR_DIFFICULTY_CHECK) {
        return false;
    }

    double totalTime = 0.0;

    // 2. Check if all recent answers were correct AND fast enough.
    for (const auto &answerRecord: m_answerHistory) {
        // answerRecord.first is bool isCorrect
        // answerRecord.second is double timeInSeconds

        // If any answer was incorrect (shouldn't happen if `recordAnswer` clears on incorrect)
        // or if it was too slow, then no streak is present.
        if (!answerRecord.first || answerRecord.second > QUICK_ANSWER_THRESHOLD_S) {
            return false;
        }
        totalTime += answerRecord.second;
    }

    // Since `recordAnswer` already clears history on incorrect answers,
    // if we reach here, all answers in `m_answerHistory` are correct.
    // We just need to check if they are all within the time limit.
    // The loop above already covers the time check, so if we're here, it's true.

    return true; // All recent answers were correct and fast, and threshold not yet set.
}

// New methods for threshold tracking
bool Player::hasThresholdBeenSet() const {
    return m_thresholdSet;
}

void Player::setThresholdSet(bool set) {
    m_thresholdSet = set;
}