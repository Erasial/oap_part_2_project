#pragma once

#include "Person.hpp"

#include <QString>

enum class RoleType {
	Admin = 1,
	Doctor = 2
};

class User : public Person {
public:
	User() = default;
	User(int id, const QString& fullName, const QString& username, const QString& passwordHash, RoleType role);

	const QString& username() const;
	void setUsername(const QString& username);

	const QString& passwordHash() const;
	void setPasswordHash(const QString& passwordHash);

	RoleType role() const;
	void setRole(RoleType role);

private:
	QString m_username;
	QString m_passwordHash;
	RoleType m_role = RoleType::Doctor;
};
