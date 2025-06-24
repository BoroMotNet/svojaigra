#include "Player.h"

Player::Player(const QString &name)
    : m_name(name),
      m_score(0),
      m_pointsThreshold(0),
      m_thresholdSet(false)
{
}

QString Player::getName() const { return m_name; }
int Player::getScore() const { return m_score; }
void Player::addScore(int points) { m_score += points; }
void Player::setPointsThreshold(int threshold) { m_pointsThreshold = threshold; }
int Player::getPointsThreshold() const { return m_pointsThreshold; }
bool Player::hasThresholdBeenSet() const { return m_thresholdSet; }
void Player::setThresholdSet(bool set) { m_thresholdSet = set; }

void Player::clearAnswerHistory() {
    m_answerHistory.clear();
}

void Player::resetScore() {
    m_score = 0;
    m_answerHistory.clear();
    m_pointsThreshold = 0;
    m_thresholdSet = false;
}


void Player::recordAnswer(bool isCorrect, double timeInSeconds) {
    if (m_thresholdSet) {
        return;
    }

    if (!isCorrect) {
        m_answerHistory.clear();
        return;
    }

    m_answerHistory.emplace_back(true, timeInSeconds);

    if (m_answerHistory.size() > HISTORY_SIZE_FOR_DIFFICULTY_CHECK) {
        m_answerHistory.erase(m_answerHistory.begin());
    }

    if (m_answerHistory.size() < HISTORY_SIZE_FOR_DIFFICULTY_CHECK) {
        return;
    }

    bool triggerThreshold = true;
    for (const auto &answerRecord : m_answerHistory) {
        if (answerRecord.second > QUICK_ANSWER_THRESHOLD_S) {
            triggerThreshold = false;
            break;
        }
    }

    if (triggerThreshold) {
        m_thresholdSet = true;
    }
}

bool Player::shouldLockEasyQuestions() const {
    return m_thresholdSet;
}