#include "../core/FileManager.h"
#include "../core/Question.h"

#include <QFile>
#include <QTextStream>
#include <QStandardPaths>
#include <QDir>
#include <QDateTime>
#include <QDebug> // Для вывода сообщений об ошибках

// Для работы с JSON
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>

// Для работы с настройками
#include <QSettings>

#include "Player.h"

namespace FileManager {
    std::vector<Question> loadQuestions(const QString &languageCode) {
        std::vector<Question> questions;

        // Формируем путь к файлу в системе ресурсов Qt.
        // ":/" - префикс для ресурсов.
        // Файлы должны быть добавлены в resources.qrc
        const QString fileName = QString(":/questions/questions_%1.json").arg(languageCode);

        QFile file(fileName);

        if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
            qWarning() << "Не удалось открыть файл с вопросами:" << fileName;
            return questions; // Возвращаем пустой вектор
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
        // Находим кросс-платформенный путь для сохранения данных приложения
        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
        QDir dir(dataPath);

        // Если папки не существует, создаем ее
        if (!dir.exists()) {
            dir.mkpath(".");
        }

        QString filePath = dataPath + "/game_results.txt";

        // Открываем файл для дозаписи в конец
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
        // QSettings автоматически обрабатывает местоположение и формат файла
        QSettings settings; // Имена организации и приложения мы задали в main.cpp
        settings.setValue("language", languageCode);
    }

    QString loadLanguageSetting() {
        QSettings settings;
        // Возвращаем сохраненное значение, или "ru" если ничего не найдено
        return settings.value("language", "en").toString();
    }
} // namespace FileManager
