#pragma once

#include "model/User.hpp"

#include <QtSql/QSqlDatabase>

class UserRepository {
public:
    explicit UserRepository(QSqlDatabase db);

    bool create(const User& user, int& newId);
    bool update(const User& user);
    bool remove(int userId);

    bool findById(int userId, User& user) const;
    bool findByUsername(const QString& username, User& user) const;

private:
    QSqlDatabase m_db;
};
