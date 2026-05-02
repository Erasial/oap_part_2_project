#include "UserRepository.hpp"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

UserRepository::UserRepository(QSqlDatabase db)
    : m_db(db) {}

bool UserRepository::create(const User& user, int& newId) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO users (username, password_hash, full_name, role_id) "
        "VALUES (?, ?, ?, ?)"
    );
    query.addBindValue(user.username());
    query.addBindValue(user.passwordHash());
    query.addBindValue(user.fullName());
    query.addBindValue(static_cast<int>(user.role()));

    if (!query.exec()) {
        return false;
    }

    newId = query.lastInsertId().toInt();
    return true;
}

bool UserRepository::update(const User& user) {
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE users SET username = ?, password_hash = ?, full_name = ?, role_id = ? "
        "WHERE id = ?"
    );
    query.addBindValue(user.username());
    query.addBindValue(user.passwordHash());
    query.addBindValue(user.fullName());
    query.addBindValue(static_cast<int>(user.role()));
    query.addBindValue(user.id());

    return query.exec();
}

bool UserRepository::remove(int userId) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = ?");
    query.addBindValue(userId);
    return query.exec();
}

bool UserRepository::findById(int userId, User& user) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, username, password_hash, full_name, role_id FROM users WHERE id = ?");
    query.addBindValue(userId);
    if (!query.exec() || !query.next()) {
        return false;
    }

    user.setId(query.value(0).toInt());
    user.setUsername(query.value(1).toString());
    user.setPasswordHash(query.value(2).toString());
    user.setFullName(query.value(3).toString());
    user.setRole(static_cast<RoleType>(query.value(4).toInt()));

    return true;
}

bool UserRepository::findByUsername(const QString& username, User& user) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, username, password_hash, full_name, role_id FROM users WHERE username = ?");
    query.addBindValue(username);
    if (!query.exec() || !query.next()) {
        return false;
    }

    user.setId(query.value(0).toInt());
    user.setUsername(query.value(1).toString());
    user.setPasswordHash(query.value(2).toString());
    user.setFullName(query.value(3).toString());
    user.setRole(static_cast<RoleType>(query.value(4).toInt()));

    return true;
}
