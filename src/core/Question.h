#ifndef QUESTION_H
#define QUESTION_H

#include <QString>

/**
 * @enum QuestionType
 * @brief Определяет тип медиа-контента, связанного с вопросом.
 */
enum class QuestionType {
    Text,         // Текст
    ImageAndText, // Текст и изображение
    SoundAndText  // Текст и звук
};

/**
 * @struct Question
 * @brief Структура для хранения всех данных одного вопроса.
 *
 * Является простым контейнером данных. Вся информация публична
 * и предназначена для прямого доступа.
 */
struct Question
{
    // --- Поля данных ---

    int id = -1;                     ///< Уникальный идентификатор вопроса из базы данных (JSON)
    QString category;                ///< Категория, к которой относится вопрос (напр. "История")
    int points = 0;                  ///< Стоимость вопроса в очках (100, 200, ...)

    QuestionType type = QuestionType::Text; ///< Тип вопроса (текст, картинка, звук)
    QString questionText;            ///< Сам текст вопроса
    QString answerText;              ///< Правильный текстовый ответ
    QString mediaPath;               ///< Путь к медиа-файлу (картинке или звуку)

    // --- Конструкторы ---

    /**
     * @brief Конструктор по умолчанию.
     * Создает пустой, неинициализированный вопрос.
     */
    Question() = default;


    // --- Вспомогательные методы ---

    /**
     * @brief Проверяет, является ли вопрос мультимедийным.
     * @return true, если тип вопроса - ImageAndText или SoundAndText.
     */
    bool isMultimedia() const;

    /**
     * @brief Сравнивает ответ пользователя с правильным ответом.
     * Сравнение происходит без учета регистра и начальных/конечных пробелов.
     * @param userAnswer Ответ, введенный пользователем.
     * @return true, если ответ правильный, иначе false.
     */
    bool checkAnswer(const QString& userAnswer) const;
};

#endif // QUESTION_H