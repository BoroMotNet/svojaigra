#ifndef USERMANAGER_H
#define USERMANAGER_H

#include <QString>
#include <QList>

struct UserScore {
    QString username;
    int totalScore;
};

class UserManager
{
public:
    static UserManager& instance();

    bool userExists(const QString& username);
    bool registerUser(const QString& username, const QString& password);
    bool validateUser(const QString& username, const QString& password);
    void addUserScore(const QString& username, int score);

    QList<UserScore> getAllUserScores() const;

private:
    UserManager() = default;
    ~UserManager() = default;
    UserManager(const UserManager&) = delete;
    UserManager& operator=(const UserManager&) = delete;

    QString hashPassword(const QString& password, const QString& salt) const;
};

#endif