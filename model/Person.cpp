#include "Person.hpp"

Person::Person(int id, const QString& fullName)
	: m_id(id),
	  m_fullName(fullName) {}

int Person::id() const {
	return m_id;
}

void Person::setId(int id) {
	m_id = id;
}

const QString& Person::fullName() const {
	return m_fullName;
}

void Person::setFullName(const QString& fullName) {
	m_fullName = fullName;
}
