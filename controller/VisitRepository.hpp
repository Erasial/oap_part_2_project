#pragma once

#include "model/Visit.hpp"

#include <QtSql/QSqlDatabase>
#include <QVector>

class VisitRepository {
public:
    explicit VisitRepository(QSqlDatabase db);

    bool create(const Visit& visit, int& newId);
    bool update(const Visit& visit);
    bool remove(int visitId);

    bool findById(int visitId, Visit& visit) const;
    QVector<Visit> listAll() const;
    QVector<Visit> listByDoctor(int doctorId) const;
    QVector<Visit> listByPatient(int patientId) const;

private:
    bool loadVisit(QSqlQuery& query, Visit& visit) const;
    bool replaceVisitLinks(int visitId, const QString& tableName, const QVector<int>& ids, const QString& idColumn) const;
    bool loadVisitLinks(int visitId, const QString& tableName, const QString& idColumn, QVector<int>& outIds) const;

    QSqlDatabase m_db;
};
