#pragma once

#include <QString>
#include <QtSql/QSqlDatabase>

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool initialize(const QString& databasePath, const QString& schemaPath, const QString& seedDirPath);
    bool verifyBootstrap(QString* errorMessage = nullptr) const;
    QSqlDatabase database() const;

private:
    DatabaseManager() = default;

    bool ensureSchema(const QString& schemaPath);
    bool enablePragmas();
    bool seedLookups(const QString& seedDirPath);

    QSqlDatabase m_db;
};
