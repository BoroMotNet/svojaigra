#include "MainWindow.h" // Включаем заголовочный файл для этого класса

// Реализация конструктора MainWindow
MainWindow::MainWindow(QWidget *parent) : QWidget(parent)
{
    // Здесь будет код вашего игрового окна.
    // Пока что просто установим заголовок и размер.
    setWindowTitle(tr("Своя Игра"));
    resize(1024, 768);
}