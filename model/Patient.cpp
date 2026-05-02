#include "Patient.hpp"

Patient::Patient(int id, const QString& fullName, const QString& firstName, const QString& lastName, const QDate& birthdate, const QString& phone)
	: Person(id, fullName),
	  m_firstName(firstName),
	  m_lastName(lastName),
	  m_birthdate(birthdate),
	  m_phone(phone) {}

const QString& Patient::firstName() const {
	return m_firstName;
}

void Patient::setFirstName(const QString& firstName) {
	m_firstName = firstName;
}

const QString& Patient::lastName() const {
	return m_lastName;
}

void Patient::setLastName(const QString& lastName) {
	m_lastName = lastName;
}

const QDate& Patient::birthdate() const {
	return m_birthdate;
}

void Patient::setBirthdate(const QDate& birthdate) {
	m_birthdate = birthdate;
}

const QString& Patient::phone() const {
	return m_phone;
}

void Patient::setPhone(const QString& phone) {
	m_phone = phone;
}
