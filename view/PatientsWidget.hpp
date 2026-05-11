#pragma once

#include <QWidget>

#include "controller/DoctorController.hpp"
#include "controller/LookupRepository.hpp"
#include "controller/PatientController.hpp"
#include "controller/SessionContext.hpp"
#include "controller/VisitController.hpp"

class QLineEdit;
class QTableWidget;
class QPushButton;

class PatientsWidget : public QWidget {
    Q_OBJECT

public:
    explicit PatientsWidget(PatientController& controller,
                            VisitController& visitController,
                            DoctorController& doctorController,
                            LookupRepository& lookupRepository,
                            SessionContext& session,
                            QWidget* parent = nullptr);

private slots:
    void loadPatients();
    void applyFilter();
    void handleAdd();
    void handleUpdate();
    void handleDelete();
    void openPatientVisits();

private:
    int selectedPatientId() const;
    bool matchesFilter(const Patient& patient, const QString& filter) const;
    void updateActionStates();

    PatientController& m_controller;
    VisitController& m_visitController;
    DoctorController& m_doctorController;
    LookupRepository& m_lookupRepository;
    SessionContext& m_session;
    QTableWidget* m_table = nullptr;
    QLineEdit* m_filterEdit = nullptr;
    QPushButton* m_addButton = nullptr;
    QPushButton* m_updateButton = nullptr;
    QPushButton* m_deleteButton = nullptr;
    QVector<Patient> m_allPatients;
};
