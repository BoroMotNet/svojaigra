#include "Question.h"

// Реализация конструктора
Question::Question() : id(0), points(0), type(TextOnly), answered(false) {}

// Реализация метода проверки ответа
bool Question::checkAnswer(const QString &userAnswer) const {
    return userAnswer.trimmed().toLower() == answer.trimmed().toLower();
}

// Реализация метода проверки мультимедиа
bool Question::isMultimedia() const {
    return type == ImageAndText || type == SoundAndText;
}