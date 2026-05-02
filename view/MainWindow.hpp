#pragma once

#include <QMainWindow>

class DoctorController;
class LookupRepository;
class PasswordHasher;
class PatientController;
class SessionContext;
class VisitController;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(SessionContext& session,
               PatientController& patientController,
               VisitController& visitController,
               DoctorController& doctorController,
               LookupRepository& lookupRepository,
               const PasswordHasher& hasher,
               QWidget* parent = nullptr);

private:
    SessionContext& m_session;
    PatientController& m_patientController;
    VisitController& m_visitController;
    DoctorController& m_doctorController;
    LookupRepository& m_lookupRepository;
    const PasswordHasher& m_hasher;
};
