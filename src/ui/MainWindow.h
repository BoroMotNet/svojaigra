#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <map>
#include "../core/Question.h"
#include "../core/Player.h"
#include <QKeyEvent>

class QGridLayout;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void updateBoardState();
    void updatePlayerInfo(int playerIndex);
    void updatePlayerScore(int playerIndex, int newScore);
    void displayQuestion(const Question& question);
    void handleQuestionClicked();
    void showGameResults(const std::vector<Player>& finalResults);

    // New slots for the buttons
    void handleRandomQuestionClicked();
    void handleExitClicked();

private:
    void setupUi();
    void setupPlayerBar();
    void setupGameBoard();
    void applyCurrentSettings();
    void keyPressEvent(QKeyEvent *event) override;

    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_playerLayout;
    QGridLayout* m_boardLayout;

    // New buttons
    QPushButton* m_randomQuestionButton;
    QPushButton* m_exitButton;

    std::vector<QLabel*> m_playerNameLabels;
    std::vector<QLabel*> m_playerScoreLabels;
    std::map<QString, std::map<int, QPushButton*>> m_questionButtons;
};

#endif //MAINWINDOW_H