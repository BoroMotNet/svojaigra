#include "./StartWindow.h"
#include "ui_StartWindow.h"

#include "./MainWindow.h"
#include "./AdminEditor.h"
#include "./PlayerNameDialog.h"
#include "./SettingsDialog.h"
#include "../core/GameManager.h"

#include <QApplication>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>

StartWindow::StartWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::StartWindow)
{
    ui->setupUi(this);

    connect(ui->startButton, &QPushButton::clicked, this, &StartWindow::handleStart);
    connect(ui->settingsButton, &QPushButton::clicked, this, &StartWindow::handleSettings);
    connect(ui->editButton, &QPushButton::clicked, this, &StartWindow::handleEdit);
    connect(ui->exitButton, &QPushButton::clicked, QApplication::instance(), &QApplication::quit); // Эта кнопка всегда закрывает приложение

    setWindowTitle(tr("Своя Игра - Меню"));

    // Инициализируем указатели на окна игры и редактора как nullptr
    // Эти окна будут создаваться при необходимости
    gameWindow = nullptr;
    adminWindow = nullptr;
}

StartWindow::~StartWindow()
{
    delete ui;
    // Поскольку MainWindow и AdminEditor теперь создаются с StartWindow как родителем,
    // Qt автоматически удалит их, когда удалится StartWindow.
    // Если бы они не имели родителя, их нужно было бы удалять здесь вручную:
    // delete gameWindow;
    // delete adminWindow;
}

void StartWindow::handleStart()
{
    PlayerNameDialog dialog(4, this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList playerNames = dialog.getPlayerNames();
        GameManager::instance().startGame(playerNames);

        // Создаем MainWindow, если его еще нет, и передаем текущее StartWindow как родителя
        if (!gameWindow) {
            gameWindow = new MainWindow(this); // <-- Передаем 'this' (StartWindow) как родителя
        }
        gameWindow->showFullScreen(); // Показываем окно игры

        this->hide(); // <-- Скрываем StartWindow вместо закрытия
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
            if (!adminWindow) {
                adminWindow = new AdminEditor(this); // <-- Передаем 'this' (StartWindow) как родителя
            }
            adminWindow->showFullScreen(); // Показываем окно редактора
            this->hide(); // <-- Скрываем StartWindow вместо закрытия
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Неверный пароль!"));
        }
    }
}