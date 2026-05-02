#pragma once

#include "model/Doctor.hpp"

#include <QtSql/QSqlDatabase>
#include <QVector>

class DoctorRepository {
public:
    explicit DoctorRepository(QSqlDatabase db);

    bool create(const Doctor& doctor, int& newId);
    bool update(const Doctor& doctor);
    bool remove(int doctorId);

    bool findById(int doctorId, Doctor& doctor) const;
    QVector<Doctor> listAll() const;

private:
    bool loadDoctor(QSqlQuery& query, Doctor& doctor) const;

    QSqlDatabase m_db;
};
