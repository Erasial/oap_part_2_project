#pragma once

#include "User.hpp"

class Admin : public User {
public:
	Admin() = default;
	Admin(int id, const QString& fullName, const QString& username, const QString& passwordHash);
};
