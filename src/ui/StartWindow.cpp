#include "./StartWindow.h"
#include "./MainWindow.h"
#include "./AdminEditor.h"
#include "./PlayerNameDialog.h"
#include "./SettingsDialog.h"
#include "../core/GameManager.h"

#include <QApplication>
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
    startButton = new QPushButton(tr("Пачаць"));
    settingsButton = new QPushButton(tr("Налады"));
    editButton = new QPushButton(tr("Рэдагаванне"));
    exitButton = new QPushButton(tr("Выхад"));

    QFont buttonFont("Arial", 14);
    startButton->setFont(buttonFont);
    settingsButton->setFont(buttonFont);
    editButton->setFont(buttonFont);
    exitButton->setFont(buttonFont);

    startButton->setMinimumHeight(50);
    settingsButton->setMinimumHeight(50);
    editButton->setMinimumHeight(50);
    exitButton->setMinimumHeight(50);

    mainLayout = new QVBoxLayout(this);
    mainLayout->addWidget(startButton);
    mainLayout->addWidget(settingsButton);
    mainLayout->addWidget(editButton);
    mainLayout->addWidget(exitButton);

    connect(startButton, &QPushButton::clicked, this, &StartWindow::handleStart);
    connect(settingsButton, &QPushButton::clicked, this, &StartWindow::handleSettings);
    connect(editButton, &QPushButton::clicked, this, &StartWindow::handleEdit);
    connect(exitButton, &QPushButton::clicked, QApplication::instance(), &QApplication::quit);

    setWindowTitle(tr("Своя Игра - Меню"));
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
        gameWindow->showFullScreen();
        
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
            adminWindow->showFullScreen();
            this->close();
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Неверный пароль!"));
        }
    }
}