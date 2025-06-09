#include "./PlayerNameDialog.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QDialogButtonBox>
#include <QPushButton>

PlayerNameDialog::PlayerNameDialog(int playerCount, QWidget *parent)
    : QDialog(parent)
{
    setWindowTitle(tr("Введите имена игроков"));
    setModal(true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    QFormLayout *formLayout = new QFormLayout();

    for (int i = 0; i < playerCount; ++i) {
        QLineEdit *lineEdit = new QLineEdit(this);
        lineEdit->setFont(QFont("Arial", 12));
        connect(lineEdit, &QLineEdit::textChanged, this, &PlayerNameDialog::validateNames);
        m_nameEdits.push_back(lineEdit);
        formLayout->addRow(tr("Игрок %1:").arg(i + 1), lineEdit);
    }

    m_buttonBox = new QDialogButtonBox(this);
    m_startButton = m_buttonBox->addButton(tr("Начать игру"), QDialogButtonBox::AcceptRole);
    m_buttonBox->addButton(tr("Назад"), QDialogButtonBox::RejectRole);

    connect(m_buttonBox, &QDialogButtonBox::accepted, this, &PlayerNameDialog::accept);
    connect(m_buttonBox, &QDialogButtonBox::rejected, this, &PlayerNameDialog::reject);

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(m_buttonBox);
    
    validateNames();
}

QStringList PlayerNameDialog::getPlayerNames() const
{
    QStringList names;
    for (const auto& lineEdit : m_nameEdits) {
        names << lineEdit->text().trimmed();
    }
    return names;
}

void PlayerNameDialog::validateNames()
{
    bool allNamesValid = true;
    for (const auto& lineEdit : m_nameEdits) {
        if (lineEdit->text().trimmed().isEmpty()) {
            allNamesValid = false;
            break;
        }
    }
    m_startButton->setEnabled(allNamesValid);
}