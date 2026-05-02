#include "VisitsWidget.hpp"

#include <QComboBox>
#include <QDateTimeEdit>
#include <QFormLayout>
#include <QHeaderView>
#include <QCheckBox>
#include <QLineEdit>
#include <QListWidget>
#include <QMessageBox>
#include <QPlainTextEdit>
#include <QPushButton>
#include <QSet>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>

VisitsWidget::VisitsWidget(VisitController& visitController,
                           PatientController& patientController,
                           DoctorController& doctorController,
                           LookupRepository& lookupRepository,
                           SessionContext& session,
                           QWidget* parent)
    : QWidget(parent),
      m_visitController(visitController),
      m_patientController(patientController),
      m_doctorController(doctorController),
      m_lookupRepository(lookupRepository),
      m_session(session) {
    auto* layout = new QVBoxLayout(this);

    m_visitFilter = new QLineEdit(this);
    m_visitFilter->setPlaceholderText("Search visits...");

    m_myVisitsOnly = new QCheckBox("Only my visits", this);
    if (!m_session.isDoctor()) {
        m_myVisitsOnly->setChecked(false);
        m_myVisitsOnly->setEnabled(false);
    }

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"ID", "Patient", "Doctor", "Date", "Duration", "Notes"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    auto* formLayout = new QFormLayout();

    m_patientCombo = new QComboBox(this);
    m_doctorCombo = new QComboBox(this);
    m_visitDateEdit = new QDateTimeEdit(this);
    m_visitDateEdit->setCalendarPopup(true);
    m_visitDateEdit->setDisplayFormat("yyyy-MM-dd HH:mm");

    m_durationSpin = new QSpinBox(this);
    m_durationSpin->setRange(1, 600);
    m_durationSpin->setSuffix(" min");

    m_notesEdit = new QPlainTextEdit(this);
    m_notesEdit->setFixedHeight(80);

    formLayout->addRow("Patient", m_patientCombo);
    formLayout->addRow("Doctor", m_doctorCombo);
    formLayout->addRow("Visit date", m_visitDateEdit);
    formLayout->addRow("Duration", m_durationSpin);
    formLayout->addRow("Notes", m_notesEdit);

    auto* lookupLayout = new QHBoxLayout();
    auto* diagnosisLayout = new QVBoxLayout();
    auto* reasonLayout = new QVBoxLayout();
    auto* procedureLayout = new QVBoxLayout();

    m_diagnosisFilter = new QLineEdit(this);
    m_diagnosisFilter->setPlaceholderText("Filter ICD-10...");
    m_reasonFilter = new QLineEdit(this);
    m_reasonFilter->setPlaceholderText("Filter ICPC-2...");
    m_procedureFilter = new QLineEdit(this);
    m_procedureFilter->setPlaceholderText("Filter ACHI...");
    m_diagnosisList = new QListWidget(this);
    m_diagnosisList->setSelectionMode(QAbstractItemView::MultiSelection);
    m_reasonList = new QListWidget(this);
    m_reasonList->setSelectionMode(QAbstractItemView::MultiSelection);
    m_procedureList = new QListWidget(this);
    m_procedureList->setSelectionMode(QAbstractItemView::MultiSelection);

    diagnosisLayout->addWidget(m_diagnosisFilter);
    diagnosisLayout->addWidget(m_diagnosisList);
    reasonLayout->addWidget(m_reasonFilter);
    reasonLayout->addWidget(m_reasonList);
    procedureLayout->addWidget(m_procedureFilter);
    procedureLayout->addWidget(m_procedureList);

    lookupLayout->addLayout(diagnosisLayout);
    lookupLayout->addLayout(reasonLayout);
    lookupLayout->addLayout(procedureLayout);

    auto* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Add", this);
    m_updateButton = new QPushButton("Update", this);
    m_deleteButton = new QPushButton("Delete", this);
    auto* clearButton = new QPushButton("Clear", this);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_updateButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);

    layout->addWidget(m_visitFilter);
    layout->addWidget(m_myVisitsOnly);
    layout->addWidget(m_table);
    layout->addLayout(formLayout);
    layout->addLayout(lookupLayout);
    layout->addLayout(buttonLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &VisitsWidget::onSelectionChanged);
    connect(m_addButton, &QPushButton::clicked, this, &VisitsWidget::handleAdd);
    connect(m_updateButton, &QPushButton::clicked, this, &VisitsWidget::handleUpdate);
    connect(m_deleteButton, &QPushButton::clicked, this, &VisitsWidget::handleDelete);
    connect(clearButton, &QPushButton::clicked, this, &VisitsWidget::clearForm);
    connect(m_visitFilter, &QLineEdit::textChanged, this, &VisitsWidget::applyVisitFilter);
    connect(m_myVisitsOnly, &QCheckBox::toggled, this, &VisitsWidget::applyVisitFilter);
    connect(m_diagnosisFilter, &QLineEdit::textChanged, this, &VisitsWidget::applyLookupFilters);
    connect(m_reasonFilter, &QLineEdit::textChanged, this, &VisitsWidget::applyLookupFilters);
    connect(m_procedureFilter, &QLineEdit::textChanged, this, &VisitsWidget::applyLookupFilters);

    loadPatients();
    loadDoctors();
    loadLookups();
    loadVisits();
    updateActionStates();
}

void VisitsWidget::loadVisits() {
    m_allVisits = m_visitController.listVisits();
    applyVisitFilter();
}

void VisitsWidget::applyVisitFilter() {
    const QString filter = m_visitFilter->text().trimmed();
    QVector<Visit> filtered;

    const bool onlyMine = m_myVisitsOnly->isEnabled() && m_myVisitsOnly->isChecked();
    for (const Visit& visit : m_allVisits) {
        if (onlyMine && visit.doctorId() != m_session.currentUser().id()) {
            continue;
        }
        if (filter.isEmpty() || matchesVisitFilter(visit, filter)) {
            filtered.append(visit);
        }
    }

    m_table->setRowCount(filtered.size());
    for (int row = 0; row < filtered.size(); ++row) {
        const Visit& visit = filtered.at(row);

        auto* idItem = new QTableWidgetItem(QString::number(visit.id()));
        const QString patientName = m_patientNames.value(visit.patientId(), QString("%1").arg(visit.patientId()));
        const QString doctorName = m_doctorNames.value(visit.doctorId(), QString("%1").arg(visit.doctorId()));
        auto* patientItem = new QTableWidgetItem(patientName);
        auto* doctorItem = new QTableWidgetItem(doctorName);
        auto* dateItem = new QTableWidgetItem(visit.visitDate().toString("yyyy-MM-dd HH:mm"));
        auto* durationItem = new QTableWidgetItem(QString::number(visit.durationMinutes()));
        auto* notesItem = new QTableWidgetItem(visit.notes());

        m_table->setItem(row, 0, idItem);
        m_table->setItem(row, 1, patientItem);
        m_table->setItem(row, 2, doctorItem);
        m_table->setItem(row, 3, dateItem);
        m_table->setItem(row, 4, durationItem);
        m_table->setItem(row, 5, notesItem);
    }
}

void VisitsWidget::onSelectionChanged() {
    const int id = selectedVisitId();
    if (id <= 0) {
        updateActionStates();
        return;
    }

    Visit visit;
    if (!m_visitController.findVisit(id, visit)) {
        updateActionStates();
        return;
    }

    setFormFromVisit(visit);
    updateActionStates();
}

void VisitsWidget::handleAdd() {
    QString errorMessage;
    if (!validateForm(errorMessage)) {
        QMessageBox::warning(this, "Invalid data", errorMessage);
        return;
    }

    Visit visit = buildVisitFromForm(0);

    int newId = 0;
    if (!m_visitController.createVisit(visit, newId)) {
        QMessageBox::warning(this, "Create failed", "Unable to create visit.");
        return;
    }

    loadVisits();
    clearForm();
}

void VisitsWidget::handleUpdate() {
    const int id = selectedVisitId();
    if (id <= 0) {
        QMessageBox::information(this, "Update", "Select a visit to update.");
        return;
    }

    QString errorMessage;
    if (!validateForm(errorMessage)) {
        QMessageBox::warning(this, "Invalid data", errorMessage);
        return;
    }

    Visit visit = buildVisitFromForm(id);
    if (!m_visitController.updateVisit(visit)) {
        QMessageBox::warning(this, "Update failed", "Unable to update visit.");
        return;
    }

    loadVisits();
}

void VisitsWidget::handleDelete() {
    const int id = selectedVisitId();
    if (id <= 0) {
        QMessageBox::information(this, "Delete", "Select a visit to delete.");
        return;
    }

    const auto confirm = QMessageBox::question(this, "Delete", "Delete selected visit?");
    if (confirm != QMessageBox::Yes) {
        return;
    }

    if (!m_visitController.removeVisit(id)) {
        QMessageBox::warning(this, "Delete failed", "Unable to delete visit.");
        return;
    }

    loadVisits();
    clearForm();
}

void VisitsWidget::clearForm() {
    m_patientCombo->setCurrentIndex(-1);
    if (m_session.isAdmin()) {
        m_doctorCombo->setCurrentIndex(-1);
    }
    m_visitDateEdit->setDateTime(QDateTime::currentDateTime());
    m_durationSpin->setValue(30);
    m_notesEdit->clear();

    for (QListWidgetItem* item : m_diagnosisList->selectedItems()) {
        item->setSelected(false);
    }
    for (QListWidgetItem* item : m_reasonList->selectedItems()) {
        item->setSelected(false);
    }
    for (QListWidgetItem* item : m_procedureList->selectedItems()) {
        item->setSelected(false);
    }

    m_table->clearSelection();
    updateActionStates();
}

void VisitsWidget::loadPatients() {
    const QVector<Patient> patients = m_patientController.listPatients();

    m_patientCombo->clear();
    m_patientCombo->setEditable(false);
    m_patientNames.clear();

    for (const Patient& patient : patients) {
        const QString label = QString("%1 %2").arg(patient.firstName(), patient.lastName());
        const QString display = QString("%1 (ID:%2)").arg(label).arg(patient.id());
        m_patientCombo->addItem(display, patient.id());
        m_patientNames.insert(patient.id(), display);
    }
}

void VisitsWidget::loadDoctors() {
    m_doctorCombo->clear();
    m_doctorCombo->setEditable(false);
    m_doctorNames.clear();

    if (m_session.isAdmin()) {
        const QVector<Doctor> doctors = m_doctorController.listDoctors();
        for (const Doctor& doctor : doctors) {
            const QString label = QString("%1 (%2)").arg(doctor.fullName(), doctor.specialization());
            const QString display = QString("%1 [ID:%2]").arg(label).arg(doctor.id());
            m_doctorCombo->addItem(display, doctor.id());
            m_doctorNames.insert(doctor.id(), display);
        }
        m_doctorCombo->setEnabled(true);
    } else {
        const QString display = QString("%1 [ID:%2]").arg(m_session.currentUser().fullName()).arg(m_session.currentUser().id());
        m_doctorCombo->addItem(display, m_session.currentUser().id());
        m_doctorCombo->setEnabled(false);
        m_doctorNames.insert(m_session.currentUser().id(), display);
    }
}

void VisitsWidget::loadLookups() {
    m_allDiagnoses = m_lookupRepository.listIcd10();
    m_allReasons = m_lookupRepository.listIcpc2();
    m_allProcedures = m_lookupRepository.listAchi();

    applyLookupFilters();
}

void VisitsWidget::applyLookupFilters() {
    populateLookupList(m_diagnosisList, m_allDiagnoses, m_diagnosisFilter->text());
    populateLookupList(m_reasonList, m_allReasons, m_reasonFilter->text());
    populateLookupList(m_procedureList, m_allProcedures, m_procedureFilter->text());
}

void VisitsWidget::populateLookupList(QListWidget* listWidget, const QVector<LookupCode>& items, const QString& filter) const {
    listWidget->clear();

    const QString trimmed = filter.trimmed();
    for (const auto& item : items) {
        const QString label = QString("%1 - %2").arg(item.code, item.name);
        if (!trimmed.isEmpty() && !label.contains(trimmed, Qt::CaseInsensitive)) {
            continue;
        }
        auto* listItem = new QListWidgetItem(label);
        listItem->setData(Qt::UserRole, item.id);
        listWidget->addItem(listItem);
    }
}

int VisitsWidget::selectedVisitId() const {
    const QList<QTableWidgetItem*> selected = m_table->selectedItems();
    if (selected.isEmpty()) {
        return 0;
    }

    const int row = selected.first()->row();
    const QTableWidgetItem* idItem = m_table->item(row, 0);
    if (!idItem) {
        return 0;
    }

    return idItem->text().toInt();
}

Visit VisitsWidget::buildVisitFromForm(int id) const {
    Visit visit;
    visit.setId(id);
    visit.setPatientId(m_patientCombo->currentData().toInt());
    visit.setDoctorId(m_doctorCombo->currentData().toInt());
    visit.setVisitDate(m_visitDateEdit->dateTime());
    visit.setDurationMinutes(m_durationSpin->value());
    visit.setNotes(m_notesEdit->toPlainText().trimmed());

    QVector<int> diagnoses;
    for (QListWidgetItem* item : m_diagnosisList->selectedItems()) {
        diagnoses.append(item->data(Qt::UserRole).toInt());
    }

    QVector<int> reasons;
    for (QListWidgetItem* item : m_reasonList->selectedItems()) {
        reasons.append(item->data(Qt::UserRole).toInt());
    }

    QVector<int> procedures;
    for (QListWidgetItem* item : m_procedureList->selectedItems()) {
        procedures.append(item->data(Qt::UserRole).toInt());
    }

    visit.setDiagnosisIds(diagnoses);
    visit.setReasonIds(reasons);
    visit.setProcedureIds(procedures);

    return visit;
}

void VisitsWidget::setFormFromVisit(const Visit& visit) {
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

    const QSet<int> diagnosisSet = QSet<int>(visit.diagnosisIds().begin(), visit.diagnosisIds().end());
    const QSet<int> reasonSet = QSet<int>(visit.reasonIds().begin(), visit.reasonIds().end());
    const QSet<int> procedureSet = QSet<int>(visit.procedureIds().begin(), visit.procedureIds().end());

    for (int i = 0; i < m_diagnosisList->count(); ++i) {
        auto* item = m_diagnosisList->item(i);
        item->setSelected(diagnosisSet.contains(item->data(Qt::UserRole).toInt()));
    }
    for (int i = 0; i < m_reasonList->count(); ++i) {
        auto* item = m_reasonList->item(i);
        item->setSelected(reasonSet.contains(item->data(Qt::UserRole).toInt()));
    }
    for (int i = 0; i < m_procedureList->count(); ++i) {
        auto* item = m_procedureList->item(i);
        item->setSelected(procedureSet.contains(item->data(Qt::UserRole).toInt()));
    }
}

void VisitsWidget::setFormEnabled(bool enabled) {
    m_patientCombo->setEnabled(enabled);
    if (m_session.isAdmin()) {
        m_doctorCombo->setEnabled(enabled);
    }
    m_visitDateEdit->setEnabled(enabled);
    m_durationSpin->setEnabled(enabled);
    m_notesEdit->setEnabled(enabled);
    m_diagnosisList->setEnabled(enabled);
    m_reasonList->setEnabled(enabled);
    m_procedureList->setEnabled(enabled);
}

void VisitsWidget::updateActionStates() {
    const int visitId = selectedVisitId();
    bool canEdit = false;

    if (visitId > 0) {
        Visit visit;
        if (m_visitController.findVisit(visitId, visit)) {
            if (m_session.isAdmin()) {
                canEdit = true;
            } else if (m_session.isDoctor()) {
                canEdit = visit.doctorId() == m_session.currentUser().id();
            }
        }
    }

    m_updateButton->setEnabled(canEdit);
    m_deleteButton->setEnabled(canEdit);
}

bool VisitsWidget::validateForm(QString& errorMessage) const {
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

bool VisitsWidget::matchesVisitFilter(const Visit& visit, const QString& filter) const {
    const QString patientName = m_patientNames.value(visit.patientId(), QString("%1").arg(visit.patientId()));
    const QString doctorName = m_doctorNames.value(visit.doctorId(), QString("%1").arg(visit.doctorId()));
    const QString composite = QString("%1 %2 %3 %4 %5")
        .arg(patientName,
             doctorName,
             visit.visitDate().toString("yyyy-MM-dd HH:mm"),
             QString::number(visit.durationMinutes()),
             visit.notes());
    return composite.contains(filter, Qt::CaseInsensitive);
}
