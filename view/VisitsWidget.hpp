#pragma once

#include <QHash>
#include <QWidget>

#include "controller/DoctorController.hpp"
#include "controller/LookupRepository.hpp"
#include "controller/PatientController.hpp"
#include "controller/SessionContext.hpp"
#include "controller/VisitController.hpp"

class QLineEdit;
class QPushButton;
class QCheckBox;
class QTableWidget;

class VisitsWidget : public QWidget {
    Q_OBJECT

public:
    VisitsWidget(VisitController& visitController,
                PatientController& patientController,
                DoctorController& doctorController,
                LookupRepository& lookupRepository,
                SessionContext& session,
                QWidget* parent = nullptr);

private slots:
    void loadVisits();
    void applyVisitFilter();
    void handleAdd();
    void handleUpdate();
    void handleDelete();
    void openVisitEditor();

private:
    void loadPatients();
    void loadDoctors();
    bool matchesVisitFilter(const Visit& visit, const QString& filter) const;

    int selectedVisitId() const;
    void updateActionStates();

    VisitController& m_visitController;
    PatientController& m_patientController;
    DoctorController& m_doctorController;
    LookupRepository& m_lookupRepository;
    SessionContext& m_session;

    QTableWidget* m_table = nullptr;
    QLineEdit* m_visitFilter = nullptr;
    QCheckBox* m_myVisitsOnly = nullptr;

    QPushButton* m_addButton = nullptr;
    QPushButton* m_updateButton = nullptr;
    QPushButton* m_deleteButton = nullptr;

    QHash<int, QString> m_patientNames;
    QHash<int, QString> m_doctorNames;
    QVector<Visit> m_allVisits;
};
