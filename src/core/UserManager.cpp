#include "UserManager.h"
#include <QSettings>
#include <QCryptographicHash>
#include <QUuid>
#include <algorithm>

UserManager &UserManager::instance() {
    static UserManager instance;
    return instance;
}

bool UserManager::userExists(const QString &username) {
    QSettings settings;
    settings.beginGroup("Users");
    bool exists = settings.childGroups().contains(username, Qt::CaseInsensitive);
    settings.endGroup();
    return exists;
}

bool UserManager::registerUser(const QString &username, const QString &password) {
    if (userExists(username)) {
        return false;
    }

    QSettings settings;
    settings.beginGroup("Users");
    settings.beginGroup(username);

    QString salt = QUuid::createUuid().toString();
    QString passwordHash = hashPassword(password, salt);

    settings.setValue("passwordHash", passwordHash);
    settings.setValue("salt", salt);
    settings.setValue("totalScore", 0);

    settings.endGroup();
    settings.endGroup();
    return true;
}

bool UserManager::validateUser(const QString &username, const QString &password) {
    if (!userExists(username)) {
        return false;
    }

    QSettings settings;
    settings.beginGroup("Users");
    settings.beginGroup(username);

    QString storedSalt = settings.value("salt").toString();
    QString storedHash = settings.value("passwordHash").toString();

    settings.endGroup();
    settings.endGroup();

    return hashPassword(password, storedSalt) == storedHash;
}

void UserManager::addUserScore(const QString &username, int score) {
    if (!userExists(username)) return;

    QSettings settings;
    settings.beginGroup("Users");
    settings.beginGroup(username);

    int currentScore = settings.value("totalScore", 0).toInt();
    settings.setValue("totalScore", currentScore + score);

    settings.endGroup();
    settings.endGroup();
}

QString UserManager::hashPassword(const QString &password, const QString &salt) const {
    QByteArray dataToHash = (password + salt).toUtf8();
    QByteArray hash = QCryptographicHash::hash(dataToHash, QCryptographicHash::Sha256);
    return hash.toHex();
}

QList<UserScore> UserManager::getAllUserScores() const {
    QList<UserScore> scores;
    QSettings settings;
    settings.beginGroup("Users");

    for (const QString &username: settings.childGroups()) {
        settings.beginGroup(username);
        int totalScore = settings.value("totalScore", 0).toInt();
        scores.append({username, totalScore});
        settings.endGroup();
    }

    settings.endGroup();

    std::sort(scores.begin(), scores.end(), [](const UserScore &a, const UserScore &b) {
        return a.totalScore > b.totalScore;
    });

    return scores;
}
