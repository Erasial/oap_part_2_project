#include "Admin.hpp"

Admin::Admin(int id, const QString& fullName, const QString& username, const QString& passwordHash)
	: User(id, fullName, username, passwordHash, RoleType::Admin) {}
