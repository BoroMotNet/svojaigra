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

StartWindow::StartWindow(QWidget *parent) : QWidget(parent),
                                            ui(new Ui::StartWindow) {
    ui->setupUi(this);

    connect(ui->startButton, &QPushButton::clicked, this, &StartWindow::handleStart);
    connect(ui->settingsButton, &QPushButton::clicked, this, &StartWindow::handleSettings);
    connect(ui->editButton, &QPushButton::clicked, this, &StartWindow::handleEdit);
    connect(ui->exitButton, &QPushButton::clicked, QApplication::instance(), &QApplication::quit);
    setWindowTitle(tr("Своя Игра - Меню"));

    gameWindow = nullptr;
    adminWindow = nullptr;
}

StartWindow::~StartWindow() {
    delete ui;
}

void StartWindow::handleStart() {
    PlayerNameDialog dialog(4, this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList playerNames = dialog.getPlayerNames();
        GameManager::instance().startGame(playerNames);

        if (!gameWindow) {
            gameWindow = new MainWindow(this);
        }
        gameWindow->showFullScreen();

        this->hide();
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
                                             QLineEdit::Password,
                                             "", &ok);

    if (ok && !password.isEmpty()) {
        if (password == adminPassword) {
            if (!adminWindow) {
                adminWindow = new AdminEditor();
            }
            adminWindow->showFullScreen();
            this->hide();
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Неверный пароль!"));
        }
    }
}
