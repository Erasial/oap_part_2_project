#include "DatabaseManager.hpp"

#include "SeedImporter.hpp"

#include <QFile>
#include <QTextStream>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtSql/QSqlRecord>
#include <QtSql/QSqlResult>

namespace {
QString readFileContents(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    QTextStream stream(&file);
    return stream.readAll();
}

bool executeSqlScript(QSqlDatabase& db, const QString& sql) {
    QString current;
    for (const QChar ch : sql) {
        if (ch == ';') {
            const QString statement = current.trimmed();
            current.clear();
            if (statement.isEmpty()) {
                continue;
            }
            QSqlQuery query(db);
            if (!query.exec(statement)) {
                return false;
            }
        } else {
            current.append(ch);
        }
    }
    const QString trailing = current.trimmed();
    if (!trailing.isEmpty()) {
        QSqlQuery query(db);
        if (!query.exec(trailing)) {
            return false;
        }
    }
    return true;
}

bool tableExists(QSqlDatabase& db, const QString& tableName) {
    QSqlQuery query(db);
    query.prepare("SELECT name FROM sqlite_master WHERE type='table' AND name=?");
    query.addBindValue(tableName);
    if (!query.exec()) {
        return false;
    }
    return query.next();
}
}

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager instance;
    return instance;
}

bool DatabaseManager::initialize(const QString& databasePath, const QString& schemaPath, const QString& seedDirPath) {
    m_lastError.clear();
    if (m_db.isOpen()) {
        return true;
    }

    m_db = QSqlDatabase::addDatabase("QSQLITE");
    m_db.setDatabaseName(databasePath);
    if (!m_db.open()) {
        setLastError("Failed to open database: " + m_db.lastError().text() + " (" + databasePath + ")");
        return false;
    }

    if (!enablePragmas()) {
        return false;
    }

    if (!ensureSchema(schemaPath)) {
        return false;
    }

    if (!seedLookups(seedDirPath)) {
        return false;
    }

    return true;
}

QSqlDatabase DatabaseManager::database() const {
    return m_db;
}

const QString& DatabaseManager::lastError() const {
    return m_lastError;
}

bool DatabaseManager::verifyBootstrap(QString* errorMessage) const {
    if (!m_db.isOpen()) {
        if (errorMessage) {
            *errorMessage = "Database not open.";
        }
        return false;
    }

    QSqlQuery adminQuery(m_db);
    if (!adminQuery.exec(
            "SELECT COUNT(*) "
            "FROM users u "
            "JOIN roles r ON r.id = u.role_id "
            "WHERE r.role_name = 'admin'")) {
        if (errorMessage) {
            *errorMessage = "Failed to check admin users.";
        }
        return false;
    }

    if (!adminQuery.next() || adminQuery.value(0).toInt() == 0) {
        if (errorMessage) {
            *errorMessage = "No admin user found.";
        }
        return false;
    }

    const QStringList lookupTables = {"icd10_codes", "icpc2_codes", "achi_codes"};
    for (const QString& tableName : lookupTables) {
        QSqlQuery countQuery(m_db);
        if (!countQuery.exec("SELECT COUNT(*) FROM " + tableName)) {
            if (errorMessage) {
                *errorMessage = "Failed to check lookup table: " + tableName;
            }
            return false;
        }
        if (!countQuery.next() || countQuery.value(0).toInt() == 0) {
            if (errorMessage) {
                *errorMessage = "Lookup table is empty: " + tableName;
            }
            return false;
        }
    }

    return true;
}

bool DatabaseManager::enablePragmas() {
    QSqlQuery query(m_db);
    if (!query.exec("PRAGMA foreign_keys = ON")) {
        setLastError("Failed to enable foreign keys.");
        return false;
    }
    if (!query.exec("PRAGMA journal_mode = WAL")) {
        setLastError("Failed to enable WAL mode.");
        return false;
    }
    return true;
}

bool DatabaseManager::ensureSchema(const QString& schemaPath) {
    if (tableExists(m_db, "roles")) {
        return true;
    }

    const QString sql = readFileContents(schemaPath);
    if (sql.isEmpty()) {
        setLastError("Schema file missing or empty: " + schemaPath);
        return false;
    }

    if (!executeSqlScript(m_db, sql)) {
        setLastError("Failed to execute schema: " + schemaPath);
        return false;
    }

    return true;
}

bool DatabaseManager::seedLookups(const QString& seedDirPath) {
    SeedImporter importer(m_db);

    const QString icd10Path = seedDirPath + "/icd10_codes.csv";
    const QString icpc2Path = seedDirPath + "/icpc2_codes.csv";
    const QString achiPath = seedDirPath + "/achi_codes.csv";

    if (!QFile::exists(icd10Path)) {
        setLastError("Missing seed file: " + icd10Path);
        return false;
    }
    if (!QFile::exists(icpc2Path)) {
        setLastError("Missing seed file: " + icpc2Path);
        return false;
    }
    if (!QFile::exists(achiPath)) {
        setLastError("Missing seed file: " + achiPath);
        return false;
    }

    if (!importer.importIfEmpty("icd10_codes", icd10Path)) {
        setLastError("Failed to import seed file: " + icd10Path);
        return false;
    }
    if (!importer.importIfEmpty("icpc2_codes", icpc2Path)) {
        setLastError("Failed to import seed file: " + icpc2Path);
        return false;
    }
    if (!importer.importIfEmpty("achi_codes", achiPath)) {
        setLastError("Failed to import seed file: " + achiPath);
        return false;
    }
    return true;
}

void DatabaseManager::setLastError(const QString& message) {
    m_lastError = message;
}
