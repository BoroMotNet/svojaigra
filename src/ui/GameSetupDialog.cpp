#include "GameSetupDialog.h"
#include "../core/UserManager.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>
#include <QCheckBox>

// Конструктор и setupUi остаются без изменений...
GameSetupDialog::GameSetupDialog(QWidget *parent) : QDialog(parent)
{
    setupUi();
    setWindowTitle(tr("Подготовка к игре"));
}

void GameSetupDialog::setupUi()
{
    auto mainLayout = new QVBoxLayout(this);
    auto gridLayout = new QGridLayout();
    gridLayout->setSpacing(10);

    for (int i = 0; i < 4; ++i) {
        int row = i;
        QLabel* playerLabel = new QLabel(tr("Игрок %1:").arg(i + 1), this);
        QLineEdit* nameEdit = new QLineEdit(this);
        nameEdit->setPlaceholderText(tr("Введите имя гостя или войдите"));
        m_nameEdits.push_back(nameEdit);

        QPushButton* loginButton = new QPushButton(tr("Войти"), this);

        connect(loginButton, &QPushButton::clicked, this, [this, i]() {
            onLoginClicked(i);
        });

        connect(nameEdit, &QLineEdit::textChanged, this, &GameSetupDialog::updateStartButtonState);

        gridLayout->addWidget(playerLabel, row, 0);
        gridLayout->addWidget(nameEdit, row, 1);
        gridLayout->addWidget(loginButton, row, 2);
    }

    mainLayout->addLayout(gridLayout);
    mainLayout->addStretch();

    auto buttonLayout = new QHBoxLayout();
    QPushButton* registerButton = new QPushButton(tr("Регистрация"), this);
    QPushButton* startGameButton = new QPushButton(tr("Начать игру"), this);
    QPushButton* cancelButton = new QPushButton(tr("Отмена"), this);

    startGameButton->setObjectName("startGameButton");
    startGameButton->setEnabled(false);

    connect(registerButton, &QPushButton::clicked, this, [this]() {
        bool ok;
        QString username = QInputDialog::getText(this, tr("Регистрация"), tr("Выберите имя пользователя:"), QLineEdit::Normal, "", &ok);
        if (!ok || username.isEmpty()) return;

        if (UserManager::instance().userExists(username)) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Пользователь с таким именем уже существует."));
            return;
        }

        QString password = QInputDialog::getText(this, tr("Регистрация"), tr("Придумайте пароль:"), QLineEdit::Password, "", &ok);
        if (!ok || password.isEmpty()) return;

        if (UserManager::instance().registerUser(username, password)) {
            QMessageBox::information(this, tr("Успех"), tr("Регистрация прошла успешно! Теперь вы можете войти."));
        } else {
            QMessageBox::critical(this, tr("Ошибка"), tr("Не удалось зарегистрировать пользователя."));
        }
    });

    connect(startGameButton, &QPushButton::clicked, this, &GameSetupDialog::onStartGameClicked);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);

    buttonLayout->addWidget(registerButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(startGameButton);
    buttonLayout->addWidget(cancelButton);
    mainLayout->addLayout(buttonLayout);
}

void GameSetupDialog::onLoginClicked(int playerIndex)
{
    bool ok;
    QString username = QInputDialog::getText(this, tr("Вход"), tr("Имя пользователя:"), QLineEdit::Normal, "", &ok);
    if (!ok || username.isEmpty()) return;

    for (int i = 0; i < m_nameEdits.size(); ++i) {
        if (m_nameEdits[i]->text().compare(username, Qt::CaseInsensitive) == 0 && m_nameEdits[i]->isReadOnly()) {
            QMessageBox::warning(this, tr("Ошибка"), tr("Этот пользователь уже в игре!"));
            return;
        }
    }

    QString password = QInputDialog::getText(this, tr("Вход"), tr("Пароль:"), QLineEdit::Password, "", &ok);
    if (!ok || password.isEmpty()) return;

    if (UserManager::instance().validateUser(username, password)) {
        m_nameEdits[playerIndex]->setText(username);
        m_nameEdits[playerIndex]->setReadOnly(true);
    } else {
        QMessageBox::warning(this, tr("Ошибка"), tr("Неверное имя пользователя или пароль."));
    }
    updateStartButtonState();
}


// --- ИЗМЕНЕНИЯ ЗДЕСЬ ---
void GameSetupDialog::onStartGameClicked()
{
    m_playerNames.clear();
    QStringList lobbyNamesForDuplicateCheck;

    for (int i = 0; i < m_nameEdits.size(); ++i) {
        QLineEdit* nameEdit = m_nameEdits[i];
        QString currentName = nameEdit->text().trimmed();

        if (currentName.isEmpty()) {
            continue; // Пропускаем пустые слоты
        }

        // 1. Проверка на дубликаты имен внутри самого лобби
        if (lobbyNamesForDuplicateCheck.contains(currentName, Qt::CaseInsensitive)) {
            QMessageBox::warning(this, tr("Дубликат имени"),
                                 tr("Имена игроков должны быть уникальными. Игрок '%1' указан несколько раз.")
                                 .arg(currentName));
            return;
        }
        lobbyNamesForDuplicateCheck.append(currentName);

        // 2. ГЛАВНАЯ ПРОВЕРКА: если игрок - гость (поле не ReadOnly)...
        if (!nameEdit->isReadOnly()) {
            // ...проверяем, не занято ли его имя зарегистрированным пользователем
            if (UserManager::instance().userExists(currentName)) {
                QMessageBox::warning(this, tr("Имя занято"),
                                     tr("Имя '%1' уже используется зарегистрированным пользователем. "
                                        "Пожалуйста, выберите другое имя для гостя или войдите под этим именем.")
                                     .arg(currentName));
                return; // Прерываем запуск игры
            }
        }

        // Если все проверки пройдены, добавляем имя в итоговый список
        m_playerNames.append(currentName);
    }

    if (m_playerNames.isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка"), tr("Добавьте хотя бы одного игрока."));
        return;
    }

    accept(); // Все проверки пройдены, закрываем диалог и начинаем игру
}

void GameSetupDialog::updateStartButtonState()
{
    bool atLeastOnePlayer = false;
    for (const auto& nameEdit : m_nameEdits) {
        if (!nameEdit->text().trimmed().isEmpty()) {
            atLeastOnePlayer = true;
            break;
        }
    }
    QPushButton* startButton = findChild<QPushButton*>("startGameButton");
    if (startButton) {
        startButton->setEnabled(atLeastOnePlayer);
    }
}

QStringList GameSetupDialog::getPlayerNames() const
{
    return m_playerNames;
}

bool GameSetupDialog::isGuestGame() const
{
    for (const auto& nameEdit : m_nameEdits) {
        // Если хотя бы одно поле заблокировано (т.е. игрок вошел в систему),
        // то игра уже не считается полностью гостевой.
        if (nameEdit->isReadOnly()) {
            return false;
        }
    }
    // Если все поля доступны для редактирования - значит, все игроки гости.
    return true;
}