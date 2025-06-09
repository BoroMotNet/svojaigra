#include "Player.h" // Включаем собственный заголовочный файл

/**
 * @brief Конструктор класса Player.
 * Инициализирует игрока с заданным именем и нулевым счетом.
 */
Player::Player(const QString &name)
    : m_name(name), // Инициализируем имя через список инициализации
      m_score(0),
      m_pointsThreshold(0) {
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
 */
void Player::resetScore() {
    m_score = 0;
}

/**
 * @brief Записывает результат ответа игрока в его историю.
 * Если история становится слишком длинной, самый старый результат удаляется.
 */
void Player::recordAnswer(bool isCorrect, double timeInSeconds) {
    // Добавляем новый результат в конец вектора
    m_answerHistory.emplace_back(isCorrect, timeInSeconds);

    // Если размер истории превысил установленный лимит,
    // удаляем самый первый (самый старый) элемент.
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
 */
bool Player::shouldLockEasyQuestions() const {
    // 1. Проверяем, достаточно ли у нас данных для анализа.
    // Если игрок ответил меньше чем на N вопросов, усложнять рано.
    if (m_answerHistory.size() < HISTORY_SIZE_FOR_DIFFICULTY_CHECK) {
        return false;
    }

    double totalTime = 0.0;

    // 2. Проверяем, были ли все последние ответы правильными.
    for (const auto &answerRecord: m_answerHistory) {
        // Если хотя бы один ответ был неверным, усложнять не нужно.
        if (!answerRecord.first) {
            // .first - это bool isCorrect
            return false;
        }
        totalTime += answerRecord.second; // .second - это double timeInSeconds
    }

    // 3. Если все ответы были верными, проверяем среднее время.
    double averageTime = totalTime / m_answerHistory.size();

    // Если среднее время меньше нашего порога, значит игрок отвечает быстро и правильно.
    if (averageTime < QUICK_ANSWER_THRESHOLD_S) {
        return true;
    }

    return false;
}
