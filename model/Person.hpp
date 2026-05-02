/*
 * Base entity with common identity fields.
 */
#pragma once

#include <QString>

class Person {
public:
	Person() = default;
	Person(int id, const QString& fullName);

	int id() const;
	void setId(int id);

	const QString& fullName() const;
	void setFullName(const QString& fullName);

private:
	int m_id = 0;
	QString m_fullName;
};
