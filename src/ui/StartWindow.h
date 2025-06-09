#ifndef STARTWINDOW_H
#define STARTWINDOW_H

#include <QWidget> // Базовый класс для всех виджетов

// Прямое объявление (Forward Declaration) классов, которые мы будем использовать.
// Это позволяет нам использовать указатели на них без включения полных заголовочных файлов,
// что ускоряет компиляцию и избегает циклических зависимостей.
class QPushButton;
class QVBoxLayout;
class MainWindow;
class AdminEditor;

class StartWindow : public QWidget
{
    // Q_OBJECT - обязательный макрос для любого класса,
    // который использует сигналы и слоты.
    Q_OBJECT

public:
    // Конструктор с возможностью указать родительский виджет.
    explicit StartWindow(QWidget *parent = nullptr);
    ~StartWindow(); // Деструктор для освобождения памяти

    // private slots - специальная секция для методов, которые будут
    // вызываться в ответ на сигналы (например, нажатие кнопки).
private slots:
    void handleStart();  // Будет вызван при нажатии на "Начать"
    void handleEdit();   // Будет вызван при нажатии на "Редактирование"

private:
    // Указатели на элементы интерфейса. Мы создадим их в .cpp файле.
    QPushButton *startButton;
    QPushButton *editButton;
    QVBoxLayout *mainLayout;

    // Указатели на другие окна, которые может создать StartWindow.
    MainWindow  *gameWindow;
    AdminEditor *adminWindow;
};

#endif // STARTWINDOW_H