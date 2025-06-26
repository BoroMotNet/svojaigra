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
#include <QSettings>
#include <QCryptographicHash>

const QString PASSWORD_SALT = "a7b3c9d1e5f6g2h4";

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

QString StartWindow::hashPassword(const QString &password) const {
    QByteArray dataToHash = (password + PASSWORD_SALT).toUtf8();
    QByteArray hash = QCryptographicHash::hash(dataToHash, QCryptographicHash::Sha256);
    return hash.toHex();
}

void StartWindow::handleStart() {
    PlayerNameDialog dialog(4, this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList playerNames = dialog.getPlayerNames();
        if(playerNames.isEmpty() || playerNames.first().isEmpty()) {
             QMessageBox::warning(this, tr("Ошибка"), tr("Введите хотя бы одно имя игрока!"));
             return;
        }

        GameManager::instance().startGame(playerNames);
        this->hide();

        auto* gameWindow = new MainWindow(playerNames, this);
        gameWindow->setAttribute(Qt::WA_DeleteOnClose);
        gameWindow->showFullScreen();
    }
}

void StartWindow::handleSettings() {
    SettingsDialog dialog(this);
    dialog.exec();
}

void StartWindow::handleEdit() {
    QSettings settings;
    QString storedHash = settings.value("admin/passwordHash").toString();

    if (storedHash.isEmpty()) {
        QMessageBox::information(this, tr("Создание пароля"), tr("Пароль администратора ещё не задан. Вам будет предложено создать новый пароль."));

        bool ok;
        QString newPassword = QInputDialog::getText(this, tr("Создание пароля"), tr("Введите новый пароль:"), QLineEdit::Password, "", &ok);

        if (ok && !newPassword.isEmpty()) {
            QString newHash = hashPassword(newPassword);
            settings.setValue("admin/passwordHash", newHash);
            QMessageBox::information(this, tr("Успех"), tr("Пароль успешно создан и сохранен."));
        } else if (ok) {
             QMessageBox::warning(this, tr("Ошибка"), tr("Пароль не может быть пустым!"));
        }
        return;
    }

    bool ok;
    QString inputPassword = QInputDialog::getText(this, tr("Авторизация"),
                                                  tr("Введите пароль администратора:"),
                                                  QLineEdit::Password, "", &ok);

    if (ok) {
        if (hashPassword(inputPassword) == storedHash) {
            this->hide();

            AdminEditor editor(this);
            editor.setWindowState(Qt::WindowFullScreen);
            editor.exec();

            this->show();
        } else {
            QMessageBox::warning(this, tr("Ошибка"), tr("Неверный пароль!"));
        }
    }
}