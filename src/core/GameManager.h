#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include <vector>
#include <QStringList>
#include <map>
#include "./Player.h"
#include "./Question.h"
#include <QElapsedTimer>
#include <QRandomGenerator>
enum class GameState {
    NotStarted,
    PlayerSelection,
    SelectingQuestion,
    AnsweringQuestion,
    GameFinished
};

class GameManager : public QObject {
    Q_OBJECT

public:
    static GameManager &instance();

    const std::vector<Player> &getPlayers() const;

    int getCurrentPlayerIndex() const;

    QStringList getCategories() const;

    const Question *getQuestion(const QString &category, int points) const;

    bool isQuestionAnswered(const QString &category, int points) const;

    bool isQuestionSelectable(const QString& category, int points) const;

    const std::map<int, Question*>& getQuestionsForCategory(const QString& category) const;

public slots:

    void startGame(const QStringList &playerNames);

    void selectQuestion(const QString &category, int points);

    void selectRandomQuestion();

    void submitAnswer(const QString &answer);

    void endGame();


signals:
    void gameStateChanged(GameState newState);

    void playerScoreUpdated(int playerIndex, int newScore);

    void currentPlayerChanged(int playerIndex);

    void questionSelected(const Question &question);

    void gameBoardUpdated();
    void gameFinished(const std::vector<Player> &finalResults);

private:
    GameManager();
    ~GameManager() = default;

    GameManager(const GameManager &) = delete;
    GameManager &operator=(const GameManager &) = delete;

    void setupBoard();

    void changeTurn();

    void checkGameEnd();

    GameState m_currentState;
    std::vector<Question> m_allQuestions;
    std::vector<Player> m_players;
    int m_currentPlayerIndex;

    Question *m_currentQuestion;

    std::map<QString, std::map<int, Question *> > m_gameBoard;

    QElapsedTimer m_answerTimer;
};

#endif