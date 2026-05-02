#include "AuthService.hpp"

AuthService::AuthService(UserRepository& userRepository, const PasswordHasher& hasher, SessionContext& session)
    : m_userRepository(userRepository),
      m_hasher(hasher),
      m_session(session) {}

bool AuthService::login(const QString& username, const QString& password) {
    User user;
    if (!m_userRepository.findByUsername(username, user)) {
        return false;
    }

    if (!m_hasher.verify(password, user.passwordHash())) {
        return false;
    }

    m_session.setCurrentUser(user);
    return true;
}

void AuthService::logout() {
    m_session.clear();
}
