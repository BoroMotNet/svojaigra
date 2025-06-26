#ifndef QUESTIONEDITDIALOG_H
#define QUESTIONEDITDIALOG_H

#include <QDialog>
#include "../core/Question.h"

class QLineEdit;
class QTextEdit;

struct MultilingualQuestionData {
    QString category;
    int points;
    Question::QuestionType type;
    QString mediaPath;

    QMap<QString, QString> displayNames;
    QMap<QString, QString> questionTexts;
    QMap<QString, QString> answerTexts;

    Question toQuestion(const QString& lang) const {
        Question q;
        q.category = category;
        q.points = points;
        q.type = type;
        q.mediaPath = mediaPath;
        q.questionText = questionTexts.value(lang, "");
        q.answer = answerTexts.value(lang, "");
        q.answered = false;
        return q;
    }
};

class QuestionEditDialog : public QDialog
{
    Q_OBJECT

public:
    explicit QuestionEditDialog(MultilingualQuestionData& questionData, QWidget* parent = nullptr);
    bool isMarkedForDeletion() const;

private slots:
    void saveChanges();
    void markForDeletion();

private:
    void setupUi();
    void loadData();

    MultilingualQuestionData& m_questionData;

    QMap<QString, QTextEdit*> m_questionEdits;
    QMap<QString, QLineEdit*> m_answerEdits;

    bool m_markedForDeletion = false;
};

#endif