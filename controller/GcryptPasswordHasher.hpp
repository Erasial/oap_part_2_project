#pragma once

#include "controller/PasswordHasher.hpp"

class GcryptPasswordHasher : public PasswordHasher {
public:
    GcryptPasswordHasher(int iterations = 120000, int saltLength = 16, int keyLength = 32);

    QString hash(const QString& plainText) const override;
    bool verify(const QString& plainText, const QString& hashed) const override;

private:
    int m_iterations;
    int m_saltLength;
    int m_keyLength;
};
