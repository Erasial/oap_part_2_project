#pragma once

#include "model/Patient.hpp"

#include <QtSql/QSqlDatabase>
#include <QVector>

class PatientRepository {
public:
    explicit PatientRepository(QSqlDatabase db);

    bool create(const Patient& patient, int& newId);
    bool update(const Patient& patient);
    bool remove(int patientId);

    bool findById(int patientId, Patient& patient) const;
    QVector<Patient> listAll() const;

private:
    QSqlDatabase m_db;
};
