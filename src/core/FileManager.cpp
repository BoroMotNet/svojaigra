#include "FileManager.h"
#include "Question.h"
#include "Player.h"
#include "../ui/QuestionEditDialog.h"

#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QThread>
#include <QJsonObject>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <QTextStream>
#include <algorithm>

namespace FileManager {
    QString getWritableQuestionsPath() {
        return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + "/questions";
    }

    bool saveAllQuestionsFromEditor(const QMap<QString, QMap<int, MultilingualQuestionData> > &allQuestions) {
        QString questionsPath = getWritableQuestionsPath();
        QDir dir(questionsPath);
        if (!dir.exists() && !dir.mkpath(".")) {
            qWarning() << "Не удалось создать директорию для сохранения:" << questionsPath;
            return false;
        }

        const QStringList languages = {"ru", "en", "be"};
        bool all_ok = true;

        for (const QString &lang: languages) {
            QJsonObject rootObjectForLang;
            for (auto it_cat = allQuestions.constBegin(); it_cat != allQuestions.constEnd(); ++it_cat) {
                const QString &categoryName = it_cat.key();
                const QMap<int, MultilingualQuestionData> &questionsInCat = it_cat.value();
                QJsonArray questionsArrayForCategory;
                for (auto it_q = questionsInCat.constBegin(); it_q != questionsInCat.constEnd(); ++it_q) {
                    const MultilingualQuestionData &qData = it_q.value();
                    if (!qData.questionTexts.contains(lang) || qData.questionTexts.value(lang).isEmpty()) continue;
                    QJsonObject qJson;
                    qJson["points"] = qData.points;
                    qJson["mediaPath"] = qData.mediaPath;
                    qJson["questionText"] = qData.questionTexts.value(lang);
                    qJson["answerText"] = qData.answerTexts.value(lang);
                    if (qData.type == Question::ImageAndText) qJson["type"] = "ImageAndText";
                    else if (qData.type == Question::SoundAndText) qJson["type"] = "SoundAndText";
                    else qJson["type"] = "Text";
                    questionsArrayForCategory.append(qJson);
                }
                if (!questionsArrayForCategory.isEmpty()) {
                    rootObjectForLang[categoryName] = questionsArrayForCategory;
                }
            }

            QFile file(QString("%1/questions_%2.json").arg(questionsPath, lang));
            if (file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text)) {
                file.write(QJsonDocument(rootObjectForLang).toJson(QJsonDocument::Indented));
                file.close();
            } else {
                qWarning() << "!!! ОШИБКА СОХРАНЕНИЯ ФАЙЛА:" << file.fileName();
                qWarning() << "!!! ПРИЧИНА:" << file.errorString();
                all_ok = false;
            }
        }
        return all_ok;
    }

    QMap<QString, QMap<int, MultilingualQuestionData> > loadAllQuestionsForEditor() {
        QMap<QString, QMap<int, MultilingualQuestionData> > allQuestions;
        const QString questionsPath = getWritableQuestionsPath();

        QFile ru_file(questionsPath + "/questions_ru.json");
        QFile en_file(questionsPath + "/questions_en.json");
        QFile be_file(questionsPath + "/questions_be.json");

        if (!ru_file.open(QIODevice::ReadOnly) || !en_file.open(QIODevice::ReadOnly) || !be_file.open(
                QIODevice::ReadOnly)) {
            qWarning() << "Не удалось открыть один из файлов вопросов для редактора.";
            return allQuestions;
        }

        QJsonObject root_ru = QJsonDocument::fromJson(ru_file.readAll()).object();
        QJsonObject root_en = QJsonDocument::fromJson(en_file.readAll()).object();
        QJsonObject root_be = QJsonDocument::fromJson(be_file.readAll()).object();
        ru_file.close();
        en_file.close();
        be_file.close();

        QMap<QString, QMap<QString, QString> > categoryTranslations;
        categoryTranslations["География"]["en"] = "Geography";
        categoryTranslations["География"]["be"] = "Геаграфія";
        categoryTranslations["Наука"]["en"] = "Science";
        categoryTranslations["Наука"]["be"] = "Навука";
        categoryTranslations["История"]["en"] = "History";
        categoryTranslations["История"]["be"] = "Гісторыя";
        categoryTranslations["Литература"]["en"] = "Literature";
        categoryTranslations["Литература"]["be"] = "Літаратура";
        categoryTranslations["Искусство"]["en"] = "Art";
        categoryTranslations["Искусство"]["be"] = "Мастацтва";


        for (const QString &canonicalCategory: root_ru.keys()) {
            QJsonArray questions_ru = root_ru.value(canonicalCategory).toArray();

            QString category_en = categoryTranslations.value(canonicalCategory).value("en", canonicalCategory);
            QString category_be = categoryTranslations.value(canonicalCategory).value("be", canonicalCategory);

            QJsonArray questions_en = root_en.value(category_en).toArray();
            QJsonArray questions_be = root_be.value(category_be).toArray();

            for (int i = 0; i < questions_ru.size(); ++i) {
                QJsonObject q_ru = questions_ru[i].toObject();
                int points = q_ru["points"].toInt();

                MultilingualQuestionData &data = allQuestions[canonicalCategory][points];

                data.category = canonicalCategory;
                data.points = points;
                data.mediaPath = q_ru["mediaPath"].toString();
                QString typeStr = q_ru["type"].toString("Text");
                if (typeStr == "ImageAndText") data.type = Question::ImageAndText;
                else if (typeStr == "SoundAndText") data.type = Question::SoundAndText;
                else data.type = Question::TextOnly;

                data.questionTexts["ru"] = q_ru["questionText"].toString();
                data.answerTexts["ru"] = q_ru["answerText"].toString();

                if (i < questions_en.size()) {
                    QJsonObject q_en = questions_en[i].toObject();
                    data.questionTexts["en"] = q_en["questionText"].toString();
                    data.answerTexts["en"] = q_en["answerText"].toString();
                }
                if (i < questions_be.size()) {
                    QJsonObject q_be = questions_be[i].toObject();
                    data.questionTexts["be"] = q_be["questionText"].toString();
                    data.answerTexts["be"] = q_be["answerText"].toString();
                }
            }
        }

        return allQuestions;
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
                if (QFile::copy(resourceFile, destFile)) {
                    QFile(destFile).setPermissions(QFileDevice::ReadOwner | QFileDevice::WriteOwner |
                                                   QFileDevice::ReadUser | QFileDevice::WriteUser |
                                                   QFileDevice::ReadGroup | QFileDevice::WriteGroup |
                                                   QFileDevice::ReadOther | QFileDevice::WriteOther);
                    qDebug() << "Скопирован файл:" << destFile;
                } else {
                    qWarning() << "Не удалось скопировать файл из ресурсов:" << resourceFile;
                }
            } else {
                qWarning() << "Файл вопросов не найден в ресурсах:" << resourceFile;
            }
        }
    }

    QMap<QString, std::vector<Question> > loadGroupedQuestionsForGame(const QString &languageCode) {
        QMap<QString, std::vector<Question> > groupedQuestions;
        QString filePath = QString("%1/questions_%2.json").arg(getWritableQuestionsPath(), languageCode);
        QFile file(filePath);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            // Если в AppData нет файла, пытаемся загрузить из ресурсов
            file.setFileName(QString(":/questions/questions_%1.json").arg(languageCode));
            if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
                qWarning() << "Не удалось открыть файл вопросов ни в AppData, ни в ресурсах:" << languageCode;
                return groupedQuestions;
            }
        }

        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        file.close();

        QJsonObject rootObject = doc.object();
        // Итерируемся по ключам корневого объекта (это наши категории)
        for (auto it = rootObject.constBegin(); it != rootObject.constEnd(); ++it) {
            QString category = it.key();
            QJsonArray qArray = it.value().toArray();
            std::vector<Question> questionsInCategory;

            // Итерируемся по вопросам в массиве категории
            for (const QJsonValue &value: qArray) {
                QJsonObject obj = value.toObject();
                Question q;
                q.category = category; // Категория теперь берется из ключа
                q.points = obj["points"].toInt();
                q.questionText = obj["questionText"].toString();
                q.answer = obj["answerText"].toString();
                q.mediaPath = obj["mediaPath"].toString();
                q.answered = false; // По умолчанию вопрос не отвечен

                QString typeStr = obj["type"].toString();
                if (typeStr == "ImageAndText") q.type = Question::ImageAndText;
                else if (typeStr == "SoundAndText") q.type = Question::SoundAndText;
                else q.type = Question::TextOnly;

                questionsInCategory.push_back(q);
            }
            // Сортируем вопросы по очкам, чтобы они шли в правильном порядке
            std::sort(questionsInCategory.begin(), questionsInCategory.end(), [](const Question &a, const Question &b) {
                return a.points < b.points;
            });

            groupedQuestions[category] = questionsInCategory;
        }
        return groupedQuestions;
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
            qWarning() << "Не удалось открыть файл для сохранения результатов:" << filePath;
            return false;
        }

        QTextStream out(&file);
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
} // namespace FileManager
