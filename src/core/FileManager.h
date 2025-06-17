#ifndef FILEMANAGER_H
#define FILEMANAGER_H

#include <vector>
#include "./Question.h"
#include "./Player.h"

namespace FileManager
{
    std::vector<Question> loadQuestions(const QString& languageCode);

    bool saveGameResults(const std::vector<Player>& players);

    void saveLanguageSetting(const QString& languageCode);

    QString loadLanguageSetting();
}

#endif