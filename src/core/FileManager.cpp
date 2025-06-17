#include "../core/FileManager.h"
#include "../core/Question.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug>

#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

#include <QSettings>

#include "Player.h"

namespace FileManager {
    std::vector<Question> loadQuestions(const QString &languageCode) {
        std::vector<Question> questions;

        const QString fileName = QString(":/questions/questions_%1.json").arg(languageCode);

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл с вопросами:" << fileName;
            return questions;
        }

        QByteArray fileData = file.readAll();
        file.close();

        QJsonParseError parseError;
        QJsonDocument doc = QJsonDocument::fromJson(fileData, &parseError);

        if (parseError.error != QJsonParseError::NoError) {
            qWarning() << "Ошибка парсинга JSON:" << parseError.errorString();
            return questions;
        }

        if (!doc.isArray()) {
            qWarning() << "Корневой элемент JSON - не массив.";
            return questions;
        }

        QJsonArray jsonArray = doc.array();

        for (const QJsonValue &value: jsonArray) {
            QJsonObject obj = value.toObject();
            Question q;

            q.id = obj["id"].toInt();
            q.category = obj["category"].toString();
            q.points = obj["points"].toInt();
            q.questionText = obj["questionText"].toString();
            q.answerText = obj["answerText"].toString();
            q.mediaPath = obj["mediaPath"].toString();

            QString typeStr = obj["type"].toString();
            if (typeStr == "ImageAndText") {
                q.type = QuestionType::ImageAndText;
            } else if (typeStr == "SoundAndText") {
                q.type = QuestionType::SoundAndText;
            } else {
                q.type = QuestionType::Text;
            }

            questions.push_back(q);
        }

        qInfo() << "Загружено" << questions.size() << "вопросов из файла" << fileName;
        return questions;
    }

    bool saveGameResults(const std::vector<Player> &players) {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataPath);

        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString filePath = dataPath + "/game_results.txt";

        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл для записи результатов:" << filePath;
            return false;
        }

        QTextStream out(&file);
        out.setEncoding(QStringConverter::Utf8);

        out << "GameDate: " << QDateTime::currentDateTime().toString(Qt::ISODate) << "\n";
        for (const auto &player: players) {
            out << "Player: " << player.getName() << ", Score: " << player.getScore() << "\n";
        }
        out << "--------------------\n";

        file.close();
        return true;
    }

    void saveLanguageSetting(const QString &languageCode) {
        QSettings settings;
        settings.setValue("language", languageCode);
    }

    QString loadLanguageSetting() {
        QSettings settings;
        return settings.value("language", "en").toString();
    }
}
