#ifndef GAMESETUPDIALOG_H
#define GAMESETUPDIALOG_H

#include <QDialog>
#include <QStringList>

class QLineEdit;
class QCheckBox;

class GameSetupDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GameSetupDialog(QWidget *parent = nullptr);
    QStringList getPlayerNames() const;
    bool isGuestGame() const;

private slots:
    void onLoginClicked(int playerIndex);
    void onStartGameClicked();
    void updateStartButtonState();

private:
    void setupUi();

    std::vector<QLineEdit*> m_nameEdits;
    std::vector<QCheckBox*> m_guestCheckBoxes;
    QStringList m_playerNames;
    bool m_isGuestGame = true;
};

#endif