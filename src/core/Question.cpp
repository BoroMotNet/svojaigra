#include "Question.h"

/**
 * @brief Проверяет, является ли вопрос мультимедийным.
 */
bool Question::isMultimedia() const
{
    return type == QuestionType::ImageAndText || type == QuestionType::SoundAndText;
}

/**
 * @brief Сравнивает ответ пользователя с правильным ответом.
 */
bool Question::checkAnswer(const QString &userAnswer) const // TODO: Логика совпадения
{
    return userAnswer.trimmed().toLower() == answerText.trimmed().toLower();
}