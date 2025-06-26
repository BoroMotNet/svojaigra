#include "StartWindow.h"
#include "ui_StartWindow.h"

// Подключаем все наши диалоговые окна
#include "GameSetupDialog.h"
#include "LeaderboardDialog.h"
#include "SettingsDialog.h"
#include "AdminEditor.h"

#include "../core/GameManager.h"
#include "../core/UserManager.h"

#include <QApplication>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QSettings>
#include <QCryptographicHash>

#include "MainWindow.h"

StartWindow::StartWindow(QWidget *parent) : QWidget(parent), ui(new Ui::StartWindow) {
    ui->setupUi(this);
    // Соединяем все кнопки с их обработчиками
    connect(ui->startButton, &QPushButton::clicked, this, &StartWindow::handleStart);
    connect(ui->settingsButton, &QPushButton::clicked, this, &StartWindow::handleSettings);
    connect(ui->editButton, &QPushButton::clicked, this, &StartWindow::handleEdit);
    connect(ui->leaderboardButton, &QPushButton::clicked, this, &StartWindow::handleLeaderboard); // Соединение для таблицы лидеров
    connect(ui->exitButton, &QPushButton::clicked, QApplication::instance(), &QApplication::quit);

    setWindowTitle(tr("Своя Игра - Меню"));
}

StartWindow::~StartWindow() {
    delete ui;
}

// Хеширование пароля для входа в панель администратора
QString StartWindow::hashPassword(const QString &password) const {
    const QString PASSWORD_SALT = "a7b3c9d1e5f6g2h4";
    QByteArray dataToHash = (password + PASSWORD_SALT).toUtf8();
    QByteArray hash = QCryptographicHash::hash(dataToHash, QCryptographicHash::Sha256);
    return hash.toHex();
}

// Открывает лобби для настройки игры
void StartWindow::handleStart() {
    GameSetupDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        QStringList playerNames = dialog.getPlayerNames();
        bool isGuestGame = dialog.isGuestGame();

        GameManager::instance().startGame(playerNames, isGuestGame);
        this->hide();

        auto* gameWindow = new MainWindow(this);
        gameWindow->setAttribute(Qt::WA_DeleteOnClose);
        gameWindow->showFullScreen();
    }
}

// Открывает настройки
void StartWindow::handleSettings() {
    SettingsDialog dialog(this);
    dialog.exec();
}

// *** ВОТ НЕДОСТАЮЩАЯ РЕАЛИЗАЦИЯ ***
// Открывает таблицу лидеров
void StartWindow::handleLeaderboard()
{
    LeaderboardDialog dialog(this);
    dialog.exec();
}

// Открывает редактор с проверкой пароля администратора
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
    QString inputPassword = QInputDialog::getText(this, tr("Авторизация"), tr("Введите пароль администратора:"), QLineEdit::Password, "", &ok);

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