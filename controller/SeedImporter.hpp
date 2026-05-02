#pragma once

#include <QString>
#include <QtSql/QSqlDatabase>

class SeedImporter {
public:
    explicit SeedImporter(QSqlDatabase db);

    bool importIfEmpty(const QString& tableName, const QString& csvPath);

private:
    bool importTwoColumnCsv(const QString& tableName, const QString& csvPath);

    QSqlDatabase m_db;
};
