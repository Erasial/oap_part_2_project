#include "DoctorRepository.hpp"

#include <QtSql/QSqlQuery>
#include <QtSql/QSqlError>

DoctorRepository::DoctorRepository(QSqlDatabase db)
    : m_db(db) {}

bool DoctorRepository::create(const Doctor& doctor, int& newId) {
    if (!m_db.transaction()) {
        return false;
    }

    QSqlQuery userQuery(m_db);
    userQuery.prepare(
        "INSERT INTO users (username, password_hash, full_name, role_id) "
        "VALUES (?, ?, ?, ?)"
    );
    userQuery.addBindValue(doctor.username());
    userQuery.addBindValue(doctor.passwordHash());
    userQuery.addBindValue(doctor.fullName());
    userQuery.addBindValue(static_cast<int>(RoleType::Doctor));

    if (!userQuery.exec()) {
        m_db.rollback();
        return false;
    }

    newId = userQuery.lastInsertId().toInt();

    QSqlQuery profileQuery(m_db);
    profileQuery.prepare("INSERT INTO doctor_profiles (user_id, specialization) VALUES (?, ?)");
    profileQuery.addBindValue(newId);
    profileQuery.addBindValue(doctor.specialization());

    if (!profileQuery.exec()) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool DoctorRepository::update(const Doctor& doctor) {
    if (!m_db.transaction()) {
        return false;
    }

    QSqlQuery userQuery(m_db);
    userQuery.prepare(
        "UPDATE users SET username = ?, password_hash = ?, full_name = ? "
        "WHERE id = ?"
    );
    userQuery.addBindValue(doctor.username());
    userQuery.addBindValue(doctor.passwordHash());
    userQuery.addBindValue(doctor.fullName());
    userQuery.addBindValue(doctor.id());

    if (!userQuery.exec()) {
        m_db.rollback();
        return false;
    }

    QSqlQuery profileQuery(m_db);
    profileQuery.prepare("UPDATE doctor_profiles SET specialization = ? WHERE user_id = ?");
    profileQuery.addBindValue(doctor.specialization());
    profileQuery.addBindValue(doctor.id());

    if (!profileQuery.exec()) {
        m_db.rollback();
        return false;
    }

    return m_db.commit();
}

bool DoctorRepository::remove(int doctorId) {
    QSqlQuery query(m_db);
    query.prepare("DELETE FROM users WHERE id = ? AND role_id = ?");
    query.addBindValue(doctorId);
    query.addBindValue(static_cast<int>(RoleType::Doctor));
    return query.exec();
}

bool DoctorRepository::findById(int doctorId, Doctor& doctor) const {
    QSqlQuery query(m_db);
    query.prepare(
        "SELECT u.id, u.username, u.password_hash, u.full_name, p.specialization "
        "FROM users u "
        "JOIN doctor_profiles p ON p.user_id = u.id "
        "WHERE u.id = ? AND u.role_id = ?"
    );
    query.addBindValue(doctorId);
    query.addBindValue(static_cast<int>(RoleType::Doctor));

    if (!query.exec() || !query.next()) {
        return false;
    }

    doctor.setId(query.value(0).toInt());
    doctor.setUsername(query.value(1).toString());
    doctor.setPasswordHash(query.value(2).toString());
    doctor.setFullName(query.value(3).toString());
    doctor.setSpecialization(query.value(4).toString());

    return true;
}

QVector<Doctor> DoctorRepository::listAll() const {
    QVector<Doctor> doctors;

    QSqlQuery query(m_db);
    query.prepare(
        "SELECT u.id, u.username, u.password_hash, u.full_name, p.specialization "
        "FROM users u "
        "JOIN doctor_profiles p ON p.user_id = u.id "
        "WHERE u.role_id = ? "
        "ORDER BY u.full_name"
    );
    query.addBindValue(static_cast<int>(RoleType::Doctor));

    if (!query.exec()) {
        return doctors;
    }

    while (query.next()) {
        Doctor doctor;
        doctor.setId(query.value(0).toInt());
        doctor.setUsername(query.value(1).toString());
        doctor.setPasswordHash(query.value(2).toString());
        doctor.setFullName(query.value(3).toString());
        doctor.setSpecialization(query.value(4).toString());
        doctors.append(doctor);
    }

    return doctors;
}
