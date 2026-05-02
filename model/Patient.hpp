#pragma once

#include "Person.hpp"

#include <QDate>
#include <QString>

class Patient : public Person {
public:
	Patient() = default;
	Patient(int id, const QString& fullName, const QString& firstName, const QString& lastName, const QDate& birthdate, const QString& phone);

	const QString& firstName() const;
	void setFirstName(const QString& firstName);

	const QString& lastName() const;
	void setLastName(const QString& lastName);

	const QDate& birthdate() const;
	void setBirthdate(const QDate& birthdate);

	const QString& phone() const;
	void setPhone(const QString& phone);

private:
	QString m_firstName;
	QString m_lastName;
	QDate m_birthdate;
	QString m_phone;
};
