#pragma once

#include "model/User.hpp"

#include <optional>

class SessionContext {
public:
    bool isAuthenticated() const;
    void clear();

    void setCurrentUser(const User& user);
    const User& currentUser() const;

    bool isAdmin() const;
    bool isDoctor() const;

private:
    std::optional<User> m_user;
};
