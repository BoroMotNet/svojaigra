#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <vector>
#include <QMap>

// Предварительные объявления, чтобы избежать циклических зависимостей
struct Question;
struct Player;
struct MultilingualQuestionData;

namespace FileManager {

    // --- Функции для работы с директориями и файлами вопросов ---

    // Получает полный путь к папке с вопросами в AppData
    QString getWritableQuestionsPath();

    // Проверяет наличие папки с вопросами и копирует их из ресурсов при первом запуске
    void initQuestionsDirectory();

    // --- Функции для редактора вопросов (AdminEditor) ---

    // Загружает ВСЕ вопросы для ВСЕХ языков в структуру для редактирования
    QMap<QString, QMap<int, MultilingualQuestionData>> loadAllQuestionsForEditor();
    QMap<QString, std::vector<Question>> loadGroupedQuestionsForGame(const QString& languageCode);

    // Сохраняет ВСЕ вопросы из редактора в файлы JSON (questions_ru.json и т.д.)
    bool saveAllQuestionsFromEditor(const QMap<QString, QMap<int, MultilingualQuestionData>>& allQuestions);

    // --- Функции для игрового процесса ---

    // Загружает вопросы для конкретного языка для начала игры
    std::vector<Question> loadQuestionsForGame(const QString& languageCode);

    // Сохраняет результаты игры в текстовый файл
    bool saveGameResults(const std::vector<Player>& players);

    // --- Функции для настроек приложения ---

    // Сохраняет выбранный язык в настройки
    void saveLanguageSetting(const QString& languageCode);

    // Загружает язык из настроек
    QString loadLanguageSetting();

} // namespace FileManager

#endif // FILEMANAGER_H