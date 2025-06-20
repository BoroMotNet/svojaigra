#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <vector>
#include <QMap>

struct Question;
struct Player;
struct MultilingualQuestionData;

namespace FileManager {

    // --- ОБЪЯВЛЕНИЕ НЕДОСТАЮЩЕЙ ФУНКЦИИ ---
    QString getWritableQuestionsPath(); // <-- ДОБАВЬТЕ ЭТУ СТРОКУ

    // --- Остальные объявления, которые уже были ---
    void initQuestionsDirectory();
    QMap<QString, QMap<int, MultilingualQuestionData>> loadAllQuestionsForEditor();
    bool saveAllQuestionsFromEditor(const QMap<QString, QMap<int, MultilingualQuestionData>>& allQuestions);
    std::vector<Question> loadQuestionsForGame(const QString& languageCode);
    bool saveGameResults(const std::vector<Player>& players);
    void saveLanguageSetting(const QString& languageCode);
    QString loadLanguageSetting();

} // namespace FileManager

#endif // FILEMANAGER_H