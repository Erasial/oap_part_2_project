#pragma once

#include "controller/PatientRepository.hpp"
#include "controller/SessionContext.hpp"

#include <QVector>

class PatientController {
public:
    PatientController(PatientRepository& repository, SessionContext& session);

    bool createPatient(const Patient& patient, int& newId);
    bool updatePatient(const Patient& patient);
    bool removePatient(int patientId);

    bool findPatient(int patientId, Patient& patient) const;
    QVector<Patient> listPatients() const;

private:
    bool canAccess() const;

    PatientRepository& m_repository;
    SessionContext& m_session;
};
