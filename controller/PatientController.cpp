#include "PatientController.hpp"

PatientController::PatientController(PatientRepository& repository, SessionContext& session)
    : m_repository(repository),
      m_session(session) {}

bool PatientController::createPatient(const Patient& patient, int& newId) {
    if (!canAccess()) {
        return false;
    }

    return m_repository.create(patient, newId);
}

bool PatientController::updatePatient(const Patient& patient) {
    if (!canAccess()) {
        return false;
    }

    return m_repository.update(patient);
}

bool PatientController::removePatient(int patientId) {
    if (!canAccess()) {
        return false;
    }

    return m_repository.remove(patientId);
}

bool PatientController::findPatient(int patientId, Patient& patient) const {
    if (!canAccess()) {
        return false;
    }

    return m_repository.findById(patientId, patient);
}

QVector<Patient> PatientController::listPatients() const {
    if (!canAccess()) {
        return {};
    }

    return m_repository.listAll();
}

bool PatientController::canAccess() const {
    return m_session.isAdmin() || m_session.isDoctor();
}
