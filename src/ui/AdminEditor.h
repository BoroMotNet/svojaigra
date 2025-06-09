#ifndef ADMINEDITOR_H
#define ADMINEDITOR_H

#include <QWidget>

// Полное определение класса AdminEditor
class AdminEditor : public QWidget
{
    Q_OBJECT // Обязательный макрос для классов Qt с сигналами/слотами

public:
    // Объявление конструктора
    explicit AdminEditor(QWidget *parent = nullptr);
};

#endif // ADMINEDITOR_H