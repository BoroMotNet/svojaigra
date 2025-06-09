#include "./StartWindow.h"
#include "./MainWindow.h"
#include "./AdminEditor.h"
#include "./PlayerNameDialog.h"
#include "./SettingsDialog.h"
#include "../core/GameManager.h"

#include <QPushButton>
#include <QVBoxLayout>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>

StartWindow::StartWindow(QWidget *parent)
    : QWidget(parent),
      gameWindow(nullptr),
      adminWindow(nullptr)
{
    startButton = new QPushButton(tr("Начать"));
    settingsButton = new QPushButton(tr("Настройки"));
    editButton = new QPushButton(tr("Редактирование"));

    QFont buttonFont("Arial", 14);
    startButton->setFont(buttonFont);
    settingsButton->setFont(buttonFont);
    editButton->setFont(buttonFont);

    startButton->setMinimumHeight(50);
    settingsButton->setMinimumHeight(50);
    editButton->setMinimumHeight(50);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(startButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addWidget(editButton);

    connect(startButton, &QPushButton::clicked, this, &StartWindow::handleStart);
    connect(settingsButton, &QPushButton::clicked, this, &StartWindow::handleSettings);
    connect(editButton, &QPushButton::clicked, this, &StartWindow::handleEdit);

    setWindowTitle(tr("Своя Игра - Меню"));
    setFixedSize(400, 250);
}

StartWindow::~StartWindow()
{
    delete gameWindow;
    delete adminWindow;
}

void StartWindow::handleStart()
{
    PlayerNameDialog dialog(4, this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList playerNames = dialog.getPlayerNames();
        GameManager::instance().startGame(playerNames);

        gameWindow = new MainWindow();
        gameWindow->show();

        this->close();
    }
}

void StartWindow::handleSettings()
{
    SettingsDialog dialog(this);
    dialog.exec();
}

void StartWindow::handleEdit()
{
    const QString adminPassword = "Ivan";

    bool ok;
    QString password = QInputDialog::getText(this, tr("Авторизация"),
                                             tr("Введите пароль администратора:"),
                                             QLineEdit::Password,
                                             "", &ok);

    if (ok && !password.isEmpty()) {
        if (password == adminPassword) {
            adminWindow = new AdminEditor();
            adminWindow->show();
            this->close();
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Неверный пароль!"));
        }
    }
}