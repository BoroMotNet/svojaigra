#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <vector>
#include <QMap>
#include <QCloseEvent> // Добавляем для closeEvent

#include "../core/Question.h"
#include "../core/Player.h"

class QGridLayout;
class QLabel;
class QPushButton;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QKeyEvent;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    // !!! ИЗМЕНЕНИЕ: Конструктор теперь принимает имена игроков
    explicit MainWindow(const QStringList& playerNames, QWidget *parent = nullptr);
    ~MainWindow() override = default;

protected:
    // Объявляем обработчик закрытия окна
    void closeEvent(QCloseEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void updateBoardState();
    void updatePlayerInfo(int playerIndex);
    void updatePlayerScore(int playerIndex, int newScore);
    void displayQuestion(const Question& question);
    void showGameResults(const std::vector<Player>& finalResults);
    void handleRandomQuestionClicked();
    void handleExitClicked();

private:
    void setupUi();
    // Функция теперь принимает имена игроков
    void setupPlayerBar(const QStringList& playerNames);
    void setupGameBoard();
    void applyCurrentSettings();

    QWidget* m_centralWidget;
    QVBoxLayout* m_mainLayout;
    QHBoxLayout* m_playerLayout;
    QGridLayout* m_boardLayout;

    QPushButton* m_randomQuestionButton;
    QPushButton* m_exitButton;

    std::vector<QLabel*> m_playerNameLabels;
    std::vector<QLabel*> m_playerScoreLabels;

    // Карта для хранения указателей на кнопки
    QMap<QString, QMap<int, QPushButton*>> m_questionButtons;
};

#endif // MAINWINDOW_H