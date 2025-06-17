#ifndef QUESTION_H
#define QUESTION_H

#include <QString>

enum class QuestionType {
    Text,
    ImageAndText,
    SoundAndText
};

struct Question
{

    int id = -1;
    QString category;
    int points = 0;

    QuestionType type = QuestionType::Text;
    QString questionText;
    QString answerText;
    QString mediaPath;

    Question() = default;


    bool isMultimedia() const;

    bool checkAnswer(const QString& userAnswer) const;
};

#endif