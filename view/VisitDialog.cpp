#include "VisitDialog.hpp"

#include "LookupPickerWidget.hpp"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QSpinBox>
#include <QVBoxLayout>

VisitDialog::VisitDialog(PatientController& patientController,
                         DoctorController& doctorController,
                         LookupRepository& lookupRepository,
                         SessionContext& session,
                         QWidget* parent)
    : QDialog(parent),
      m_patientController(patientController),
      m_doctorController(doctorController),
      m_lookupRepository(lookupRepository),
      m_session(session) {
    setWindowTitle("Visit");
    setModal(true);

    m_patientCombo = new QComboBox(this);
    m_doctorCombo = new QComboBox(this);
    m_visitDateEdit = new QDateTimeEdit(this);
    m_visitDateEdit->setCalendarPopup(true);
    m_visitDateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");
    m_visitDateEdit->setDateTime(QDateTime::currentDateTime());

    m_durationSpin = new QSpinBox(this);
    m_durationSpin->setRange(1, 600);
    m_durationSpin->setSuffix(" min");
    m_durationSpin->setValue(30);

    m_notesEdit = new QPlainTextEdit(this);
    m_notesEdit->setFixedHeight(90);

    m_diagnosisPicker = new LookupPickerWidget(this);
    m_reasonPicker = new LookupPickerWidget(this);
    m_procedurePicker = new LookupPickerWidget(this);

    auto* formLayout = new QFormLayout();
    formLayout->addRow("Patient", m_patientCombo);
    formLayout->addRow("Doctor", m_doctorCombo);
    formLayout->addRow("Visit date", m_visitDateEdit);
    formLayout->addRow("Duration", m_durationSpin);
    formLayout->addRow("Notes", m_notesEdit);
    formLayout->addRow("Diagnoses", m_diagnosisPicker);
    formLayout->addRow("Reasons", m_reasonPicker);
    formLayout->addRow("Procedures", m_procedurePicker);

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(formLayout);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, this, [this]() {
        QString errorMessage;
        if (!validate(errorMessage)) {
            QMessageBox::warning(this, "Invalid data", errorMessage);
            return;
        }
        accept();
    });
    connect(buttons, &QDialogButtonBox::rejected, this, &QDialog::reject);

    loadPatients();
    loadDoctors();
    loadLookups();
}

void VisitDialog::setVisit(const Visit& visit) {
    m_id = visit.id();

    const int patientIndex = m_patientCombo->findData(visit.patientId());
    if (patientIndex >= 0) {
        m_patientCombo->setCurrentIndex(patientIndex);
    }

    const int doctorIndex = m_doctorCombo->findData(visit.doctorId());
    if (doctorIndex >= 0) {
        m_doctorCombo->setCurrentIndex(doctorIndex);
    }

    m_visitDateEdit->setDateTime(visit.visitDate());
    m_durationSpin->setValue(visit.durationMinutes());
    m_notesEdit->setPlainText(visit.notes());

    m_diagnosisPicker->setSelectedIds(visit.diagnosisIds());
    m_reasonPicker->setSelectedIds(visit.reasonIds());
    m_procedurePicker->setSelectedIds(visit.procedureIds());
}

Visit VisitDialog::visit() const {
    Visit visit;
    visit.setId(m_id);
    visit.setPatientId(m_patientCombo->currentData().toInt());
    visit.setDoctorId(m_doctorCombo->currentData().toInt());
    visit.setVisitDate(m_visitDateEdit->dateTime());
    visit.setDurationMinutes(m_durationSpin->value());
    visit.setNotes(m_notesEdit->toPlainText().trimmed());
    visit.setDiagnosisIds(m_diagnosisPicker->selectedIds());
    visit.setReasonIds(m_reasonPicker->selectedIds());
    visit.setProcedureIds(m_procedurePicker->selectedIds());
    return visit;
}

void VisitDialog::loadPatients() {
    const QVector<Patient> patients = m_patientController.listPatients();
    m_patientCombo->clear();
    for (const Patient& patient : patients) {
        const QString label = QString("%1 %2 (ID:%3)").arg(patient.firstName(), patient.lastName()).arg(patient.id());
        m_patientCombo->addItem(label, patient.id());
    }
}

void VisitDialog::loadDoctors() {
    m_doctorCombo->clear();
    if (m_session.isAdmin()) {
        const QVector<Doctor> doctors = m_doctorController.listDoctors();
        for (const Doctor& doctor : doctors) {
            const QString label = QString("%1 (%2) [ID:%3]").arg(doctor.fullName(), doctor.specialization()).arg(doctor.id());
            m_doctorCombo->addItem(label, doctor.id());
        }
        m_doctorCombo->setEnabled(true);
    } else {
        const QString label = QString("%1 [ID:%2]").arg(m_session.currentUser().fullName()).arg(m_session.currentUser().id());
        m_doctorCombo->addItem(label, m_session.currentUser().id());
        m_doctorCombo->setEnabled(false);
    }
}

void VisitDialog::loadLookups() {
    m_diagnosisPicker->setItems(m_lookupRepository.listIcd10(), "Type to search ICD-10...");
    m_reasonPicker->setItems(m_lookupRepository.listIcpc2(), "Type to search ICPC-2...");
    m_procedurePicker->setItems(m_lookupRepository.listAchi(), "Type to search ACHI...");
}

bool VisitDialog::validate(QString& errorMessage) const {
    if (m_patientCombo->currentIndex() < 0) {
        errorMessage = "Select a patient.";
        return false;
    }
    if (m_doctorCombo->currentIndex() < 0) {
        errorMessage = "Select a doctor.";
        return false;
    }
    if (!m_visitDateEdit->dateTime().isValid()) {
        errorMessage = "Visit date is required.";
        return false;
    }
    if (m_durationSpin->value() <= 0) {
        errorMessage = "Duration must be greater than 0.";
        return false;
    }
    return true;
}
