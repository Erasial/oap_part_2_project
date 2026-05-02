#include "SessionContext.hpp"

bool SessionContext::isAuthenticated() const {
    return m_user.has_value();
}

void SessionContext::clear() {
    m_user.reset();
}

void SessionContext::setCurrentUser(const User& user) {
    m_user = user;
}

const User& SessionContext::currentUser() const {
    return m_user.value();
}

bool SessionContext::isAdmin() const {
    return m_user.has_value() && m_user->role() == RoleType::Admin;
}

bool SessionContext::isDoctor() const {
    return m_user.has_value() && m_user->role() == RoleType::Doctor;
}
