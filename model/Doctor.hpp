#pragma once

#include "User.hpp"

#include <QString>

class Doctor : public User {
public:
	Doctor() = default;
	Doctor(int id, const QString& fullName, const QString& username, const QString& passwordHash, const QString& specialization);

	const QString& specialization() const;
	void setSpecialization(const QString& specialization);

private:
	QString m_specialization;
};
