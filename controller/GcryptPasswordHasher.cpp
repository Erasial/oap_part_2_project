#include "GcryptPasswordHasher.hpp"

#include <QByteArray>
#include <QStringList>

#include <gcrypt.h>

namespace {
void ensureGcryptInitialized() {
    static bool initialized = false;
    if (initialized) {
        return;
    }
    gcry_check_version(nullptr);
    gcry_control(GCRYCTL_DISABLE_SECMEM, 0);
    gcry_control(GCRYCTL_INITIALIZATION_FINISHED, 0);
    initialized = true;
}
}

GcryptPasswordHasher::GcryptPasswordHasher(int iterations, int saltLength, int keyLength)
    : m_iterations(iterations),
      m_saltLength(saltLength),
      m_keyLength(keyLength) {
    ensureGcryptInitialized();
}

QString GcryptPasswordHasher::hash(const QString& plainText) const {
    ensureGcryptInitialized();

    QByteArray salt(m_saltLength, 0);
    gcry_randomize(salt.data(), m_saltLength, GCRY_STRONG_RANDOM);

    QByteArray derived(m_keyLength, 0);
    const QByteArray passwordBytes = plainText.toUtf8();

    const gcry_error_t result = gcry_kdf_derive(
        passwordBytes.constData(),
        passwordBytes.size(),
        GCRY_KDF_PBKDF2,
        GCRY_MD_SHA256,
        salt.constData(),
        salt.size(),
        m_iterations,
        derived.size(),
        derived.data()
    );

    if (result != 0) {
        return QString();
    }

    const QString saltHex = QString::fromLatin1(salt.toHex());
    const QString derivedHex = QString::fromLatin1(derived.toHex());
    return QString("gcrypt$%1$%2$%3").arg(m_iterations).arg(saltHex, derivedHex);
}

bool GcryptPasswordHasher::verify(const QString& plainText, const QString& hashed) const {
    ensureGcryptInitialized();

    const QStringList parts = hashed.split('$');
    if (parts.size() != 4 || parts.at(0) != "gcrypt") {
        return false;
    }

    bool ok = false;
    const int iterations = parts.at(1).toInt(&ok);
    if (!ok || iterations <= 0) {
        return false;
    }

    const QByteArray salt = QByteArray::fromHex(parts.at(2).toLatin1());
    const QByteArray expected = QByteArray::fromHex(parts.at(3).toLatin1());
    if (salt.isEmpty() || expected.isEmpty()) {
        return false;
    }

    QByteArray derived(expected.size(), 0);
    const QByteArray passwordBytes = plainText.toUtf8();

    const gcry_error_t result = gcry_kdf_derive(
        passwordBytes.constData(),
        passwordBytes.size(),
        GCRY_KDF_PBKDF2,
        GCRY_MD_SHA256,
        salt.constData(),
        salt.size(),
        iterations,
        derived.size(),
        derived.data()
    );

    if (result != 0) {
        return false;
    }

    return derived == expected;
}
