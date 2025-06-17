#include "Question.h"

bool Question::isMultimedia() const
{
    return type == QuestionType::ImageAndText || type == QuestionType::SoundAndText;
}

bool Question::checkAnswer(const QString &userAnswer) const
{
    return userAnswer.trimmed().toLower() == answerText.trimmed().toLower();
}