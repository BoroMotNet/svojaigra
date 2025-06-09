#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include "./Question.h"
#include "./Player.h"

/**
 * @namespace FileManager
 * @brief Пространство имен, содержащее статические функции для всех операций с файлами.
 *
 * Отвечает за загрузку вопросов из JSON, сохранение результатов игры
 * и работу с настройками приложения.
 */
namespace FileManager
{
    /**
     * @brief Загружает вопросы из JSON-файла для указанного языка.
     * Файлы должны быть частью системы ресурсов Qt (qrc).
     * @param languageCode Двухбуквенный код языка ("ru", "en", "be").
     * @return Вектор объектов Question. В случае ошибки возвращает пустой вектор.
     */
    std::vector<Question> loadQuestions(const QString& languageCode);

    /**
     * @brief Сохраняет результаты завершенной игры в текстовый файл.
     * Дописывает результат в конец файла game_results.txt.
     * @param players Вектор игроков с их итоговыми счетами.
     * @return true в случае успеха, иначе false.
     */
    bool saveGameResults(const std::vector<Player>& players);

    /**
     * @brief Сохраняет настройку языка.
     * Использует QSettings для кросс-платформенного сохранения.
     * @param languageCode Двухбуквенный код языка для сохранения.
     */
    void saveLanguageSetting(const QString& languageCode);

    /**
     * @brief Загружает настройку языка.
     * @return Сохраненный двухбуквенный код языка. По умолчанию "ru".
     */
    QString loadLanguageSetting();
}

#endif // FILEMANAGER_H