#include "./StartWindow.h" // Включаем собственный заголовочный файл

// Включаем заголовочные файлы для всех классов Qt, которые мы используем
#include <QPushButton>
#include <QVBoxLayout>
#include <QFont>
#include <QInputDialog>
#include <QMessageBox>
#include <QLineEdit>

// Включаем заголовочные файлы для окон, которые мы будем создавать.
// Теперь, когда они реализованы, эти строки должны работать без ошибок.
#include "./MainWindow.h"
#include "./AdminEditor.h"


// --- Реализация конструктора ---
StartWindow::StartWindow(QWidget *parent)
    : QWidget(parent), // Вызов конструктора родительского класса
      gameWindow(nullptr),
      adminWindow(nullptr)
{
    // 1. Создание виджетов
    // tr() - функция для поддержки локализации (перевода).
    startButton = new QPushButton(tr("Начать"));
    editButton = new QPushButton(tr("Редактирование"));

    // Устанавливаем стили для кнопок, чтобы они выглядели лучше
    QFont buttonFont("Arial", 14);
    startButton->setFont(buttonFont);
    editButton->setFont(buttonFont);
    startButton->setMinimumHeight(50);
    editButton->setMinimumHeight(50);

    // 2. Создание менеджера компоновки (layout)
    // QVBoxLayout располагает виджеты друг под другом.
    mainLayout = new QVBoxLayout(this); // `this` указывает, что layout будет управлять этим окном

    // 3. Добавление виджетов в layout
    mainLayout->addWidget(startButton);
    mainLayout->addWidget(editButton);

    // 4. Соединение сигналов со слотами (привязка логики к нажатиям)
    connect(startButton, &QPushButton::clicked, this, &StartWindow::handleStart);
    connect(editButton, &QPushButton::clicked, this, &StartWindow::handleEdit);

    // 5. Настройка самого окна
    setWindowTitle(tr("Своя Игра - Меню"));
    setFixedSize(400, 200); // Зафиксируем размер, чтобы окно не растягивалось
}

// --- Реализация деструктора ---
StartWindow::~StartWindow()
{
    // Этот код не является строго обязательным, т.к. программа закроется,
    // но это хорошая практика для управления памятью в более сложных приложениях.
    delete gameWindow;
    delete adminWindow;
}

// --- Реализация слотов ---

// Слот, который выполняется при нажатии кнопки "Начать"
void StartWindow::handleStart()
{
    // Создаем и показываем главное игровое окно
    gameWindow = new MainWindow();
    gameWindow->show(); // Показать новое окно

    this->close(); // Закрыть текущее окно (меню)
}

// Слот, который выполняется при нажатии кнопки "Редактирование"
void StartWindow::handleEdit()
{
    // ВАЖНО: В реальном приложении пароль должен храниться
    // в зашифрованном виде (хеш) в файле настроек.
    const QString adminPassword = "Ivan"; //TODO: Hash

    bool ok; // Переменная для отслеживания, нажал ли пользователь "OK" или "Cancel"

    // Вызываем стандартный диалог Qt для ввода текста
    QString password = QInputDialog::getText(this, tr("Авторизация"),
                                             tr("Введите пароль администратора:"),
                                             QLineEdit::Password, // Режим ввода пароля (скрывает символы)
                                             "", &ok);

    // Если пользователь нажал "OK" и поле ввода не пустое
    if (ok && !password.isEmpty()) {
        if (password == adminPassword) {
            // Пароль верный: создаем и показываем окно администратора
            adminWindow = new AdminEditor();
            adminWindow->show();
            this->close(); // Закрыть текущее окно (меню)
        } else {
            // Пароль неверный: показываем сообщение об ошибке
            QMessageBox::warning(this, tr("Ошибка"), tr("Неверный пароль!"));
        }
    }
}