#pragma once

#include <QHash>
#include <QWidget>

#include "controller/DoctorController.hpp"
#include "controller/LookupRepository.hpp"
#include "controller/PatientController.hpp"
#include "controller/SessionContext.hpp"
#include "controller/VisitController.hpp"

class QComboBox;
class QDateTimeEdit;
class QLineEdit;
class QListWidget;
class QPlainTextEdit;
class QPushButton;
class QCheckBox;
class QSpinBox;
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
    void onSelectionChanged();
    void handleAdd();
    void handleUpdate();
    void handleDelete();
    void clearForm();
    void applyLookupFilters();

private:
    void loadPatients();
    void loadDoctors();
    void loadLookups();
    void populateLookupList(QListWidget* listWidget, const QVector<LookupCode>& items, const QString& filter) const;
    bool validateForm(QString& errorMessage) const;
    bool matchesVisitFilter(const Visit& visit, const QString& filter) const;

    int selectedVisitId() const;
    Visit buildVisitFromForm(int id) const;
    void setFormFromVisit(const Visit& visit);
    void setFormEnabled(bool enabled);
    void updateActionStates();

    VisitController& m_visitController;
    PatientController& m_patientController;
    DoctorController& m_doctorController;
    LookupRepository& m_lookupRepository;
    SessionContext& m_session;

    QTableWidget* m_table = nullptr;
    QLineEdit* m_visitFilter = nullptr;
    QCheckBox* m_myVisitsOnly = nullptr;
    QComboBox* m_patientCombo = nullptr;
    QComboBox* m_doctorCombo = nullptr;
    QDateTimeEdit* m_visitDateEdit = nullptr;
    QSpinBox* m_durationSpin = nullptr;
    QPlainTextEdit* m_notesEdit = nullptr;
    QListWidget* m_diagnosisList = nullptr;
    QListWidget* m_reasonList = nullptr;
    QListWidget* m_procedureList = nullptr;
    QLineEdit* m_diagnosisFilter = nullptr;
    QLineEdit* m_reasonFilter = nullptr;
    QLineEdit* m_procedureFilter = nullptr;

    QPushButton* m_addButton = nullptr;
    QPushButton* m_updateButton = nullptr;
    QPushButton* m_deleteButton = nullptr;

    QHash<int, QString> m_patientNames;
    QHash<int, QString> m_doctorNames;
    QVector<LookupCode> m_allDiagnoses;
    QVector<LookupCode> m_allReasons;
    QVector<LookupCode> m_allProcedures;
    QVector<Visit> m_allVisits;
};
