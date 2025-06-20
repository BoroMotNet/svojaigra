#ifndef ADMINEDITOR_H
#define ADMINEDITOR_H

#include <QWidget>
#include <QMap>
#include "QuestionEditDialog.h" // Включаем наш новый диалог

namespace Ui {
    class AdminEditor;
}

class QGridLayout;
class QPushButton;
class QScrollArea;

class AdminEditor : public QWidget
{
    Q_OBJECT

public:
    explicit AdminEditor(QWidget *parent = nullptr);
    ~AdminEditor() override;

private slots:
    void openQuestionEditor(const QString& category, int points);
    void addNewQuestion(const QString& category);
    void saveAllChanges();

private:
    void setupUi();
    void loadAllQuestions();
    void populateBoard();
    void clearBoard();
    QWidget* createQuestionWidget(const QString& category, int points);

    Ui::AdminEditor *ui; // Оставим, если захотите что-то добавить через Designer

    QGridLayout* m_boardLayout;
    QScrollArea* m_scrollArea;
    QPushButton* m_saveButton;

    // Хранилище всех вопросов, загруженных в память
    // Структура: Категория -> (Очки -> Мультиязычный вопрос)
    QMap<QString, QMap<int, MultilingualQuestionData>> m_allQuestions;
};

#endif // ADMINEDITOR_H