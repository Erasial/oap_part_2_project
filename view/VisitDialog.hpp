#pragma once

#include <QDialog>

#include "controller/DoctorController.hpp"
#include "controller/LookupRepository.hpp"
#include "controller/PatientController.hpp"
#include "controller/SessionContext.hpp"
#include "model/Visit.hpp"

class QComboBox;
class QDateTimeEdit;
class QPlainTextEdit;
class QSpinBox;

class LookupPickerWidget;

class VisitDialog : public QDialog {
    Q_OBJECT

public:
    VisitDialog(PatientController& patientController,
                DoctorController& doctorController,
                LookupRepository& lookupRepository,
                SessionContext& session,
                QWidget* parent = nullptr);

    void setVisit(const Visit& visit);
    Visit visit() const;

private:
    void loadPatients();
    void loadDoctors();
    void loadLookups();
    bool validate(QString& errorMessage) const;

    int m_id = 0;
    PatientController& m_patientController;
    DoctorController& m_doctorController;
    LookupRepository& m_lookupRepository;
    SessionContext& m_session;

    QComboBox* m_patientCombo = nullptr;
    QComboBox* m_doctorCombo = nullptr;
    QDateTimeEdit* m_visitDateEdit = nullptr;
    QSpinBox* m_durationSpin = nullptr;
    QPlainTextEdit* m_notesEdit = nullptr;
    LookupPickerWidget* m_diagnosisPicker = nullptr;
    LookupPickerWidget* m_reasonPicker = nullptr;
    LookupPickerWidget* m_procedurePicker = nullptr;
};
