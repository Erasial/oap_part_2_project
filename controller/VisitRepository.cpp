#include "VisitRepository.hpp"

#include <QtSql/QSqlQuery>

VisitRepository::VisitRepository(QSqlDatabase db)
    : m_db(db) {}

bool VisitRepository::create(const Visit& visit, int& newId) {
    if (!m_db.transaction()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO visits (patient_id, doctor_id, visit_date, duration_minutes, notes) "
        "VALUES (?, ?, ?, ?, ?)"
    );
    query.addBindValue(visit.patientId());
    query.addBindValue(visit.doctorId());
    query.addBindValue(visit.visitDate());
    query.addBindValue(visit.durationMinutes());
    query.addBindValue(visit.notes());

    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    newId = query.lastInsertId().toInt();

    if (!replaceVisitLinks(newId, "visit_diagnoses", visit.diagnosisIds(), "icd10_id")) {
        m_db.rollback();
        return false;
    }

    if (!replaceVisitLinks(newId, "visit_reasons", visit.reasonIds(), "icpc2_id")) {
        m_db.rollback();
        return false;
    }

    if (!replaceVisitLinks(newId, "visit_procedures", visit.procedureIds(), "achi_id")) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool VisitRepository::update(const Visit& visit) {
    if (!m_db.transaction()) {
        return false;
    }

    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE visits SET patient_id = ?, doctor_id = ?, visit_date = ?, duration_minutes = ?, notes = ? "
        "WHERE id = ?"
    );
    query.addBindValue(visit.patientId());
    query.addBindValue(visit.doctorId());
    query.addBindValue(visit.visitDate());
    query.addBindValue(visit.durationMinutes());
    query.addBindValue(visit.notes());
    query.addBindValue(visit.id());

    if (!query.exec()) {
        m_db.rollback();
        return false;
    }

    if (!replaceVisitLinks(visit.id(), "visit_diagnoses", visit.diagnosisIds(), "icd10_id")) {
        m_db.rollback();
        return false;
    }

    if (!replaceVisitLinks(visit.id(), "visit_reasons", visit.reasonIds(), "icpc2_id")) {
        m_db.rollback();
        return false;
    }

    if (!replaceVisitLinks(visit.id(), "visit_procedures", visit.procedureIds(), "achi_id")) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool VisitRepository::remove(int visitId) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM visits WHERE id = ?");
    query.addBindValue(visitId);
    return query.exec();
}

bool VisitRepository::findById(int visitId, Visit& visit) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, patient_id, doctor_id, visit_date, duration_minutes, notes FROM visits WHERE id = ?");
    query.addBindValue(visitId);

    if (!query.exec() || !query.next()) {
        return false;
    }

    visit.setId(query.value(0).toInt());
    visit.setPatientId(query.value(1).toInt());
    visit.setDoctorId(query.value(2).toInt());
    visit.setVisitDate(query.value(3).toDateTime());
    visit.setDurationMinutes(query.value(4).toInt());
    visit.setNotes(query.value(5).toString());

    QVector<int> diagnoses;
    QVector<int> reasons;
    QVector<int> procedures;
    if (!loadVisitLinks(visitId, "visit_diagnoses", "icd10_id", diagnoses)) {
        return false;
    }
    if (!loadVisitLinks(visitId, "visit_reasons", "icpc2_id", reasons)) {
        return false;
    }
    if (!loadVisitLinks(visitId, "visit_procedures", "achi_id", procedures)) {
        return false;
    }

    visit.setDiagnosisIds(diagnoses);
    visit.setReasonIds(reasons);
    visit.setProcedureIds(procedures);

    return true;
}

QVector<Visit> VisitRepository::listAll() const {
    QVector<Visit> visits;

    QSqlQuery query(m_db);
    if (!query.exec("SELECT id, patient_id, doctor_id, visit_date, duration_minutes, notes FROM visits ORDER BY visit_date DESC")) {
        return visits;
    }

    while (query.next()) {
        Visit visit;
        visit.setId(query.value(0).toInt());
        visit.setPatientId(query.value(1).toInt());
        visit.setDoctorId(query.value(2).toInt());
        visit.setVisitDate(query.value(3).toDateTime());
        visit.setDurationMinutes(query.value(4).toInt());
        visit.setNotes(query.value(5).toString());
        visits.append(visit);
    }

    return visits;
}

QVector<Visit> VisitRepository::listByDoctor(int doctorId) const {
    QVector<Visit> visits;

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, patient_id, doctor_id, visit_date, duration_minutes, notes "
        "FROM visits WHERE doctor_id = ? ORDER BY visit_date DESC"
    );
    query.addBindValue(doctorId);

    if (!query.exec()) {
        return visits;
    }

    while (query.next()) {
        Visit visit;
        visit.setId(query.value(0).toInt());
        visit.setPatientId(query.value(1).toInt());
        visit.setDoctorId(query.value(2).toInt());
        visit.setVisitDate(query.value(3).toDateTime());
        visit.setDurationMinutes(query.value(4).toInt());
        visit.setNotes(query.value(5).toString());
        visits.append(visit);
    }

    return visits;
}

QVector<Visit> VisitRepository::listByPatient(int patientId) const {
    QVector<Visit> visits;

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT id, patient_id, doctor_id, visit_date, duration_minutes, notes "
        "FROM visits WHERE patient_id = ? ORDER BY visit_date DESC"
    );
    query.addBindValue(patientId);

    if (!query.exec()) {
        return visits;
    }

    while (query.next()) {
        Visit visit;
        visit.setId(query.value(0).toInt());
        visit.setPatientId(query.value(1).toInt());
        visit.setDoctorId(query.value(2).toInt());
        visit.setVisitDate(query.value(3).toDateTime());
        visit.setDurationMinutes(query.value(4).toInt());
        visit.setNotes(query.value(5).toString());
        visits.append(visit);
    }

    return visits;
}

bool VisitRepository::replaceVisitLinks(int visitId, const QString& tableName, const QVector<int>& ids, const QString& idColumn) const {
    QSqlQuery deleteQuery(m_db);
    deleteQuery.prepare("DELETE FROM " + tableName + " WHERE visit_id = ?");
    deleteQuery.addBindValue(visitId);
    if (!deleteQuery.exec()) {
        return false;
    }

    if (ids.isEmpty()) {
        return true;
    }

    QSqlQuery insertQuery(m_db);
    insertQuery.prepare("INSERT INTO " + tableName + " (visit_id, " + idColumn + ") VALUES (?, ?)");

    for (int id : ids) {
        insertQuery.addBindValue(visitId);
        insertQuery.addBindValue(id);
        if (!insertQuery.exec()) {
            return false;
        }
        insertQuery.clear();
    }

    return true;
}

bool VisitRepository::loadVisitLinks(int visitId, const QString& tableName, const QString& idColumn, QVector<int>& outIds) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT " + idColumn + " FROM " + tableName + " WHERE visit_id = ?");
    query.addBindValue(visitId);

    if (!query.exec()) {
        return false;
    }

    while (query.next()) {
        outIds.append(query.value(0).toInt());
    }

    return true;
}
