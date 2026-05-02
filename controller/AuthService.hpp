#pragma once

#include "controller/PasswordHasher.hpp"
#include "controller/SessionContext.hpp"
#include "controller/UserRepository.hpp"

class AuthService {
public:
    AuthService(UserRepository& userRepository, const PasswordHasher& hasher, SessionContext& session);

    bool login(const QString& username, const QString& password);
    void logout();

private:
    UserRepository& m_userRepository;
    const PasswordHasher& m_hasher;
    SessionContext& m_session;
};
