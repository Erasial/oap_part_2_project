#pragma once

#include <QDialog>

#include "controller/DoctorController.hpp"
#include "controller/LookupRepository.hpp"
#include "controller/PatientController.hpp"
#include "controller/SessionContext.hpp"
#include "controller/VisitController.hpp"

class QTableWidget;

class PatientVisitsDialog : public QDialog {
    Q_OBJECT

public:
    PatientVisitsDialog(VisitController& visitController,
                        DoctorController& doctorController,
                        PatientController& patientController,
                        LookupRepository& lookupRepository,
                        SessionContext& session,
                        int patientId,
                        const QString& patientName,
                        QWidget* parent = nullptr);

private:
    void loadVisits();
    void openVisitEditor();
    int selectedVisitId() const;

    VisitController& m_visitController;
    DoctorController& m_doctorController;
    PatientController& m_patientController;
    LookupRepository& m_lookupRepository;
    SessionContext& m_session;
    int m_patientId = 0;
    QTableWidget* m_table = nullptr;
};
