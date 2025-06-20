#ifndef QUESTION_H
#define QUESTION_H

#include <QString>

struct Question {
    enum QuestionType {
        TextOnly,
        ImageAndText,
        SoundAndText
    };

    int id;
    QString category;
    int points;
    QString questionText;
    QString mediaPath;
    QuestionType type;
    QString answer;
    bool answered;

    Question(); // Конструктор

    // Оставляем только ОБЪЯВЛЕНИЯ методов
    bool checkAnswer(const QString& userAnswer) const;
    bool isMultimedia() const;
};

#endif // QUESTION_H