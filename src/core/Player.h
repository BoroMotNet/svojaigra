#ifndef PLAYER_H
#define PLAYER_H

#include <QString>
#include <vector>
#include <utility>
class Player {
public:
    explicit Player(const QString &name);

    QString getName() const;

    int getScore() const;

    void addScore(int points);

    void resetScore();

    void recordAnswer(bool isCorrect, double timeInSeconds);

    bool shouldLockEasyQuestions() const;

    void setPointsThreshold(int threshold);

    int getPointsThreshold() const;

    void clearAnswerHistory();

    bool hasThresholdBeenSet() const;

    void setThresholdSet(bool set);

private:

    static const int HISTORY_SIZE_FOR_DIFFICULTY_CHECK = 2;
    static constexpr double QUICK_ANSWER_THRESHOLD_S = 5.0;


    QString m_name;

    int m_score;

    std::vector<std::pair<bool, double> > m_answerHistory;

    int m_pointsThreshold;

    bool m_thresholdSet;
};

#endif