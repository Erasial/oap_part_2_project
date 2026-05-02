#include "User.hpp"

User::User(int id, const QString& fullName, const QString& username, const QString& passwordHash, RoleType role)
	: Person(id, fullName),
	  m_username(username),
	  m_passwordHash(passwordHash),
	  m_role(role) {}

const QString& User::username() const {
	return m_username;
}

void User::setUsername(const QString& username) {
	m_username = username;
}

const QString& User::passwordHash() const {
	return m_passwordHash;
}

void User::setPasswordHash(const QString& passwordHash) {
	m_passwordHash = passwordHash;
}

RoleType User::role() const {
	return m_role;
}

void User::setRole(RoleType role) {
	m_role = role;
}
