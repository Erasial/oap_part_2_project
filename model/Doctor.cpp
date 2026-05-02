#include "Doctor.hpp"

Doctor::Doctor(int id, const QString& fullName, const QString& username, const QString& passwordHash, const QString& specialization)
	: User(id, fullName, username, passwordHash, RoleType::Doctor),
	  m_specialization(specialization) {}

const QString& Doctor::specialization() const {
	return m_specialization;
}

void Doctor::setSpecialization(const QString& specialization) {
	m_specialization = specialization;
}
