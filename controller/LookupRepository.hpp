#pragma once

#include <QtSql/QSqlDatabase>
#include <QVector>
#include <QString>

struct LookupCode {
    int id = 0;
    QString code;
    QString name;
};

class LookupRepository {
public:
    explicit LookupRepository(QSqlDatabase db);

    QVector<LookupCode> listIcd10(int limit = 0) const;
    QVector<LookupCode> listIcpc2(int limit = 0) const;
    QVector<LookupCode> listAchi(int limit = 0) const;

    bool findIcd10ById(int id, LookupCode& out) const;
    bool findIcpc2ById(int id, LookupCode& out) const;
    bool findAchiById(int id, LookupCode& out) const;

private:
    QVector<LookupCode> list(const QString& tableName, int limit) const;
    bool findById(const QString& tableName, int id, LookupCode& out) const;

    QSqlDatabase m_db;
};
