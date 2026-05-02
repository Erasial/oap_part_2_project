#include "LookupRepository.hpp"

#include <QtSql/QSqlQuery>

LookupRepository::LookupRepository(QSqlDatabase db)
    : m_db(db) {}

QVector<LookupCode> LookupRepository::listIcd10(int limit) const {
    return list("icd10_codes", limit);
}

QVector<LookupCode> LookupRepository::listIcpc2(int limit) const {
    return list("icpc2_codes", limit);
}

QVector<LookupCode> LookupRepository::listAchi(int limit) const {
    return list("achi_codes", limit);
}

bool LookupRepository::findIcd10ById(int id, LookupCode& out) const {
    return findById("icd10_codes", id, out);
}

bool LookupRepository::findIcpc2ById(int id, LookupCode& out) const {
    return findById("icpc2_codes", id, out);
}

bool LookupRepository::findAchiById(int id, LookupCode& out) const {
    return findById("achi_codes", id, out);
}

QVector<LookupCode> LookupRepository::list(const QString& tableName, int limit) const {
    QVector<LookupCode> results;

    QSqlQuery query(m_db);
    QString sql = "SELECT id, code, name FROM " + tableName + " ORDER BY code";
    if (limit > 0) {
        sql += " LIMIT ?";
        query.prepare(sql);
        query.addBindValue(limit);
    } else {
        query.prepare(sql);
    }

    if (!query.exec()) {
        return results;
    }

    while (query.next()) {
        LookupCode item;
        item.id = query.value(0).toInt();
        item.code = query.value(1).toString();
        item.name = query.value(2).toString();
        results.append(item);
    }

    return results;
}

bool LookupRepository::findById(const QString& tableName, int id, LookupCode& out) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, code, name FROM " + tableName + " WHERE id = ?");
    query.addBindValue(id);

    if (!query.exec() || !query.next()) {
        return false;
    }

    out.id = query.value(0).toInt();
    out.code = query.value(1).toString();
    out.name = query.value(2).toString();

    return true;
}
