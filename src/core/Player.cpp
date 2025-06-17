#include "Player.h"
Player::Player(const QString &name)
    : m_name(name),
      m_score(0),
      m_pointsThreshold(0),
      m_thresholdSet(false)
{
}

QString Player::getName() const {
    return m_name;
}

int Player::getScore() const {
    return m_score;
}

void Player::addScore(int points) {
    m_score += points;
}

void Player::resetScore() {
    m_score = 0;
    m_answerHistory.clear();
    m_pointsThreshold = 0;
    m_thresholdSet = false;
}

void Player::recordAnswer(bool isCorrect, double timeInSeconds) {
    if (!isCorrect) {
        clearAnswerHistory();
        return;
    }

    m_answerHistory.emplace_back(isCorrect, timeInSeconds);

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

bool Player::shouldLockEasyQuestions() const {
    if (m_thresholdSet) {
        return false;
    }

    if (m_answerHistory.size() < HISTORY_SIZE_FOR_DIFFICULTY_CHECK) {
        return false;
    }

    double totalTime = 0.0;

    for (const auto &answerRecord: m_answerHistory) {

        if (!answerRecord.first || answerRecord.second > QUICK_ANSWER_THRESHOLD_S) {
            return false;
        }
        totalTime += answerRecord.second;
    }

    return true;
}

bool Player::hasThresholdBeenSet() const {
    return m_thresholdSet;
}

void Player::setThresholdSet(bool set) {
    m_thresholdSet = set;
}