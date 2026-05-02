#include "PatientRepository.hpp"

#include <QtSql/QSqlQuery>

PatientRepository::PatientRepository(QSqlDatabase db)
    : m_db(db) {}

bool PatientRepository::create(const Patient& patient, int& newId) {
    QSqlQuery query(m_db);
    query.prepare(
        "INSERT INTO patients (first_name, last_name, birthdate, phone) "
        "VALUES (?, ?, ?, ?)"
    );
    query.addBindValue(patient.firstName());
    query.addBindValue(patient.lastName());
    query.addBindValue(patient.birthdate());
    query.addBindValue(patient.phone());

    if (!query.exec()) {
        return false;
    }

    newId = query.lastInsertId().toInt();
    return true;
}

bool PatientRepository::update(const Patient& patient) {
    QSqlQuery query(m_db);
    query.prepare(
        "UPDATE patients SET first_name = ?, last_name = ?, birthdate = ?, phone = ? "
        "WHERE id = ?"
    );
    query.addBindValue(patient.firstName());
    query.addBindValue(patient.lastName());
    query.addBindValue(patient.birthdate());
    query.addBindValue(patient.phone());
    query.addBindValue(patient.id());
    return query.exec();
}

bool PatientRepository::remove(int patientId) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM patients WHERE id = ?");
    query.addBindValue(patientId);
    return query.exec();
}

bool PatientRepository::findById(int patientId, Patient& patient) const {
    QSqlQuery query(m_db);
    query.prepare("SELECT id, first_name, last_name, birthdate, phone FROM patients WHERE id = ?");
    query.addBindValue(patientId);

    if (!query.exec() || !query.next()) {
        return false;
    }

    const QString firstName = query.value(1).toString();
    const QString lastName = query.value(2).toString();

    patient.setId(query.value(0).toInt());
    patient.setFirstName(firstName);
    patient.setLastName(lastName);
    patient.setFullName(firstName + " " + lastName);
    patient.setBirthdate(query.value(3).toDate());
    patient.setPhone(query.value(4).toString());

    return true;
}

QVector<Patient> PatientRepository::listAll() const {
    QVector<Patient> patients;

    QSqlQuery query(m_db);
    if (!query.exec("SELECT id, first_name, last_name, birthdate, phone FROM patients ORDER BY last_name, first_name")) {
        return patients;
    }

    while (query.next()) {
        Patient patient;
        const QString firstName = query.value(1).toString();
        const QString lastName = query.value(2).toString();
        patient.setId(query.value(0).toInt());
        patient.setFirstName(firstName);
        patient.setLastName(lastName);
        patient.setFullName(firstName + " " + lastName);
        patient.setBirthdate(query.value(3).toDate());
        patient.setPhone(query.value(4).toString());
        patients.append(patient);
    }

    return patients;
}
