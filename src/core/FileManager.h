#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <QString>
#include <vector>
#include <QMap>

struct Question;
struct Player;
struct MultilingualQuestionData;

namespace FileManager {

    QString getWritableQuestionsPath();

    void initQuestionsDirectory();

    QMap<QString, QMap<int, MultilingualQuestionData>> loadAllQuestionsForEditor();
    QMap<QString, std::vector<Question>> loadGroupedQuestionsForGame(const QString& languageCode);

    bool saveAllQuestionsFromEditor(const QMap<QString, QMap<int, MultilingualQuestionData>>& allQuestions);

    std::vector<Question> loadQuestionsForGame(const QString& languageCode);

    bool saveGameResults(const std::vector<Player>& players);

    void saveLanguageSetting(const QString& languageCode);

    QString loadLanguageSetting();

}

#endif