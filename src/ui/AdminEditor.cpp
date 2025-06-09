#include "AdminEditor.h" // Включаем заголовочный файл для этого класса

// Реализация конструктора AdminEditor
AdminEditor::AdminEditor(QWidget *parent) : QWidget(parent)
{
    // Здесь будет код вашего редактора вопросов.
    // Пока что просто установим заголовок и размер.
    setWindowTitle(tr("Редактор вопросов"));
    resize(800, 600);
}