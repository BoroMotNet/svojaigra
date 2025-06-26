#include "Question.h"

Question::Question() : id(0), points(0), type(TextOnly), answered(false) {}

bool Question::checkAnswer(const QString &userAnswer) const {
    return userAnswer.trimmed().toLower() == answer.trimmed().toLower();
}

bool Question::isMultimedia() const {
    return type == ImageAndText || type == SoundAndText;
}