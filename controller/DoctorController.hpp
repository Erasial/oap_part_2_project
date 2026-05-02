#pragma once

#include "controller/DoctorRepository.hpp"
#include "controller/SessionContext.hpp"

#include <QVector>

class DoctorController {
public:
    DoctorController(DoctorRepository& repository, SessionContext& session);

    bool createDoctor(const Doctor& doctor, int& newId);
    bool updateDoctor(const Doctor& doctor);
    bool removeDoctor(int doctorId);

    bool findDoctor(int doctorId, Doctor& doctor) const;
    QVector<Doctor> listDoctors() const;

private:
    DoctorRepository& m_repository;
    SessionContext& m_session;
};
