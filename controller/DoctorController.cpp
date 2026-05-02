#include "DoctorController.hpp"

DoctorController::DoctorController(DoctorRepository& repository, SessionContext& session)
    : m_repository(repository),
      m_session(session) {}

bool DoctorController::createDoctor(const Doctor& doctor, int& newId) {
    if (!m_session.isAdmin()) {
        return false;
    }

    return m_repository.create(doctor, newId);
}

bool DoctorController::updateDoctor(const Doctor& doctor) {
    if (!m_session.isAdmin()) {
        return false;
    }

    return m_repository.update(doctor);
}

bool DoctorController::removeDoctor(int doctorId) {
    if (!m_session.isAdmin()) {
        return false;
    }

    return m_repository.remove(doctorId);
}

bool DoctorController::findDoctor(int doctorId, Doctor& doctor) const {
    if (!m_session.isAdmin()) {
        return false;
    }

    return m_repository.findById(doctorId, doctor);
}

QVector<Doctor> DoctorController::listDoctors() const {
    if (!m_session.isAdmin()) {
        return {};
    }

    return m_repository.listAll();
}
