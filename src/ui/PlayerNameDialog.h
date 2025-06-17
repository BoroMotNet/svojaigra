#ifndef PLAYERNAMEDIALOG_H
#define PLAYERNAMEDIALOG_H

#include <QDialog>
#include <vector>
#include <QStringList>

class QLineEdit;
class QDialogButtonBox;
class QPushButton;

class PlayerNameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PlayerNameDialog(int playerCount = 4, QWidget *parent = nullptr);
    QStringList getPlayerNames() const;

private slots:
    void validateNames();

private:
    std::vector<QLineEdit*> m_nameEdits;
    QDialogButtonBox* m_buttonBox;
    QPushButton* m_startButton;
};

#endif