#include "StartWindow.h"
#include "ui_StartWindow.h"

#include "MainWindow.h"
#include "AdminEditor.h"
#include "PlayerNameDialog.h"
#include "SettingsDialog.h"
#include "../core/GameManager.h"

#include <QApplication>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>

StartWindow::StartWindow(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::StartWindow) {
    ui->setupUi(this);

    connect(ui->startButton, &QPushButton::clicked, this, &StartWindow::handleStart);
    connect(ui->settingsButton, &QPushButton::clicked, this, &StartWindow::handleSettings);
    connect(ui->editButton, &QPushButton::clicked, this, &StartWindow::handleEdit);
    connect(ui->exitButton, &QPushButton::clicked, QApplication::instance(), &QApplication::quit);
    setWindowTitle(tr("Своя Игра - Меню"));
}

StartWindow::~StartWindow() {
    delete ui;
}

void StartWindow::handleStart() {
    // Указываем корректные аргументы для конструктора PlayerNameDialog
    PlayerNameDialog dialog(4, this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList playerNames = dialog.getPlayerNames();
        if(playerNames.isEmpty() || playerNames.first().isEmpty()) {
             QMessageBox::warning(this, tr("Ошибка"), tr("Введите хотя бы одно имя игрока!"));
             return;
        }

        GameManager::instance().startGame(playerNames);

        this->hide();

        // Создаем игровое окно, передавая ему родителя и имена игроков
        auto* gameWindow = new MainWindow(playerNames, this);
        // Этот флаг автоматически вызовет "delete gameWindow", когда окно закроется
        gameWindow->setAttribute(Qt::WA_DeleteOnClose);

        gameWindow->showFullScreen();
    }
}

void StartWindow::handleSettings() {
    SettingsDialog dialog(this);
    dialog.exec();
}

void StartWindow::handleEdit() {
    const QString adminPassword = "1";
    bool ok;
    QString password = QInputDialog::getText(this, tr("Авторизация"),
                                             tr("Введите пароль администратора:"),
                                             QLineEdit::Password, "", &ok);
    if (ok && password == adminPassword) {
        if (ok && password == adminPassword) {
            this->hide();

            AdminEditor editor(this);
            editor.exec();

            this->show();
        } else if (ok) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Неверный пароль!"));
        }
    }
}