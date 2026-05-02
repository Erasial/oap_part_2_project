#pragma once

#include <QString>

class PasswordHasher {
public:
    virtual ~PasswordHasher() = default;

    virtual QString hash(const QString& plainText) const = 0;
    virtual bool verify(const QString& plainText, const QString& hashed) const = 0;
};
