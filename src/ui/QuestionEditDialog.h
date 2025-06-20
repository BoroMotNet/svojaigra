#ifndef QUESTIONEDITDIALOG_H
#define QUESTIONEDITDIALOG_H

#include <QDialog>
#include "../core/Question.h" // Подключаем структуру вашего вопроса

class QLineEdit;
class QTextEdit;
class QComboBox;
class QLabel;

// Структура для хранения мультиязычных данных
// Вам нужно будет интегрировать ее в вашу логику загрузки/сохранения
struct MultilingualQuestionData {
    QString category;
    int points;
    Question::QuestionType type;
    QString mediaPath;

    QMap<QString, QString> questionTexts; // "ru" -> "Текст вопроса", "en" -> "Question text"
    QMap<QString, QString> answerTexts;   // "ru" -> "Ответ", "en" -> "Answer"

    // Конвертация в базовый Question для совместимости
    Question toQuestion(const QString& lang) const {
        Question q;
        q.category = category;
        q.points = points;
        q.type = type;
        q.mediaPath = mediaPath;
        q.questionText = questionTexts.value(lang, "");
        q.answer = answerTexts.value(lang, "");
        q.answered = false; // По умолчанию
        return q;
    }
};


class QuestionEditDialog : public QDialog
{
    Q_OBJECT

public:
    // Мы передаем в диалог нашу новую структуру
    explicit QuestionEditDialog(MultilingualQuestionData& questionData, QWidget* parent = nullptr);

    // Публичный флаг, чтобы AdminEditor знал, нужно ли удалять вопрос
    bool isMarkedForDeletion() const;

private slots:
    void saveChanges();
    void uploadMedia();
    void handleTypeChanged(int index);
    void markForDeletion();

private:
    void setupUi();
    void loadData(); // Загрузка данных в поля ввода

    // Ссылка на редактируемый объект. Все изменения будут происходить в нем.
    MultilingualQuestionData& m_questionData;

    // Поля для разных языков
    QMap<QString, QTextEdit*> m_questionEdits;
    QMap<QString, QLineEdit*> m_answerEdits;

    QComboBox* m_typeComboBox;
    QLabel* m_mediaPathLabel;
    QPushButton* m_uploadMediaButton;

    bool m_markedForDeletion = false;
};

#endif // QUESTIONEDITDIALOG_H