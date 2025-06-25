#include "FileManager.h"
#include "Question.h"
#include "Player.h"
#include "../ui/QuestionEditDialog.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <algorithm>

namespace FileManager {
    bool saveAllQuestionsFromEditor(const QMap<QString, QMap<int, MultilingualQuestionData> > &allQuestions) {
        QString questionsPath = getWritableQuestionsPath();
        QDir dir(questionsPath);
        if (!dir.exists() && !dir.mkpath(".")) {
            qWarning() << "Не удалось создать директорию для сохранения:" << questionsPath;
            return false;
        }

        QStringList languages = {"ru", "en", "be"};
        bool all_ok = true;

        for (const QString &lang: languages) {
            QJsonArray questionsArray;

            auto cat_it = allQuestions.constBegin();
            while (cat_it != allQuestions.constEnd()) {
                const auto &innerMap = cat_it.value();

                auto points_it = innerMap.constBegin();
                while (points_it != innerMap.constEnd()) {
                    const auto &questionData = points_it.value();

                    QJsonObject obj;
                    obj["id"] = 0;
                    obj["category"] = questionData.category;
                    obj["points"] = questionData.points;
                    obj["mediaPath"] = questionData.mediaPath;

                    if (questionData.type == Question::ImageAndText) obj["type"] = "ImageAndText";
                    else if (questionData.type == Question::SoundAndText) obj["type"] = "SoundAndText";
                    else obj["type"] = "Text";

                    obj["questionText"] = questionData.questionTexts.value(lang);
                    obj["answerText"] = questionData.answerTexts.value(lang);

                    questionsArray.append(obj);

                    ++points_it;
                }
                ++cat_it;
            }

            QFile file(QString("%1/questions_%2.json").arg(questionsPath, lang));
            if (file.open(QIODevice::WriteOnly | QIODevice::Truncate)) {
                file.write(QJsonDocument(questionsArray).toJson(QJsonDocument::Indented));
                file.close();
            } else {
                qWarning() << "!!! ОШИБКА СОХРАНЕНИЯ ФАЙЛА:" << file.fileName();
                qWarning() << "!!! ПРИЧИНА:" << file.errorString();
                all_ok = false;
            }
        }
        return all_ok;
    }

    QString getWritableQuestionsPath() {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/questions";
    }

    void initQuestionsDirectory() {
        QString questionsPath = getWritableQuestionsPath();
        QDir dir(questionsPath);
        if (dir.exists()) { return; }
        qDebug() << "Папка с вопросами не найдена. Создание и копирование из ресурсов...";
        if (!dir.mkpath(".")) {
            qWarning() << "Не удалось создать папку для вопросов:" << questionsPath;
            return;
        }
        QStringList languages = {"ru", "en", "be"};
        for (const QString &lang: languages) {
            QString resourceFile = QString(":/questions/questions_%1.json").arg(lang);
            QString destFile = QString("%1/questions_%2.json").arg(questionsPath, lang);
            if (QFile::exists(resourceFile)) {
                QFile::copy(resourceFile, destFile);
                qDebug() << "Скопирован файл:" << destFile;
            }
        }
    }

    QMap<QString, QMap<int, MultilingualQuestionData> > loadAllQuestionsForEditor() {
        QMap<QString, QMap<int, MultilingualQuestionData> > allQuestions;
        QString questionsPath = getWritableQuestionsPath();
        QJsonArray en_array, ru_array, be_array;
        QFile en_file(questionsPath + "/questions_en.json");
        if (en_file.open(QIODevice::ReadOnly)) {
            en_array = QJsonDocument::fromJson(en_file.readAll()).array();
            en_file.close();
        } else {
            qWarning() << "Не удалось загрузить основной файл 'questions_en.json'.";
            return allQuestions;
        }
        QFile ru_file(questionsPath + "/questions_ru.json");
        if (ru_file.open(QIODevice::ReadOnly)) {
            ru_array = QJsonDocument::fromJson(ru_file.readAll()).array();
            ru_file.close();
        }
        QFile be_file(questionsPath + "/questions_be.json");
        if (be_file.open(QIODevice::ReadOnly)) {
            be_array = QJsonDocument::fromJson(be_file.readAll()).array();
            be_file.close();
        }
        for (int i = 0; i < en_array.size(); ++i) {
            QJsonObject en_obj = en_array[i].toObject();
            QString canonicalCategory = en_obj["category"].toString();
            int points = en_obj["points"].toInt();

            MultilingualQuestionData &data = allQuestions[canonicalCategory][points];
            data.category = canonicalCategory;
            data.points = points;
            data.mediaPath = en_obj["mediaPath"].toString();

            QString typeStr = en_obj["type"].toString("Text");
            if (typeStr == "ImageAndText") data.type = Question::ImageAndText;
            else if (typeStr == "SoundAndText") data.type = Question::SoundAndText;
            else data.type = Question::TextOnly;

            data.questionTexts["en"] = en_obj["questionText"].toString();
            data.answerTexts["en"] = en_obj["answerText"].toString();

            if (i < ru_array.size()) {
                QJsonObject ru_obj = ru_array[i].toObject();
                data.questionTexts["ru"] = ru_obj["questionText"].toString();
                data.answerTexts["ru"] = ru_obj["answerText"].toString();
            }
            if (i < be_array.size()) {
                QJsonObject be_obj = be_array[i].toObject();
                data.questionTexts["be"] = be_obj["questionText"].toString();
                data.answerTexts["be"] = be_obj["answerText"].toString();
            }
        }
        return allQuestions;
    }

    std::vector<Question> loadQuestionsForGame(const QString &languageCode) {
        std::vector<Question> questions;
        QString filePath = QString("%1/questions_%2.json").arg(getWritableQuestionsPath(), languageCode);
        QFile file(filePath);
        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            file.setFileName(QString(":/questions/questions_%1.json").arg(languageCode));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) return questions;
        }
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();
        QJsonArray jsonArray = doc.array();
        for (const QJsonValue &value: jsonArray) {
            QJsonObject obj = value.toObject();
            Question q;
            q.category = obj["category"].toString();
            q.points = obj["points"].toInt();
            q.questionText = obj["questionText"].toString();
            q.answer = obj["answerText"].toString();
            q.mediaPath = obj["mediaPath"].toString();
            q.answered = false;
            QString typeStr = obj["type"].toString();
            if (typeStr == "ImageAndText") q.type = Question::ImageAndText;
            else if (typeStr == "SoundAndText") q.type = Question::SoundAndText;
            else q.type = Question::TextOnly;
            questions.push_back(q);
        }
        return questions;
    }

    bool saveGameResults(const std::vector<Player> &players) {
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataPath);
        if (!dir.exists()) dir.mkpath(".");
        QString filePath = dataPath + "/game_results.txt";
        QFile file(filePath);
        if (!file.open(QIODevice::WriteOnly | QIODevice::Append | QIODevice::Text)) return false;
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
        return settings.value("language", "ru").toString();
    }
}
