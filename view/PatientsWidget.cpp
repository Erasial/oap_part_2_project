#include "PatientsWidget.hpp"

#include "PatientDialog.hpp"
#include "PatientVisitsDialog.hpp"

#include <QDialog>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PatientsWidget::PatientsWidget(PatientController& controller,
                                                             VisitController& visitController,
                                                             DoctorController& doctorController,
                                                             LookupRepository& lookupRepository,
                                                             SessionContext& session,
                                                             QWidget* parent)
    : QWidget(parent),
            m_controller(controller),
            m_visitController(visitController),
            m_doctorController(doctorController),
            m_lookupRepository(lookupRepository),
            m_session(session) {
    auto* layout = new QVBoxLayout(this);

    m_filterEdit = new QLineEdit(this);
    m_filterEdit->setPlaceholderText("Search patients...");

    m_table = new QTableWidget(this);
    m_table->setColumnCount(5);
    m_table->setHorizontalHeaderLabels({"ID", "First name", "Last name", "Birthdate", "Phone"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);

    auto* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Add", this);
    m_updateButton = new QPushButton("Edit", this);
    m_deleteButton = new QPushButton("Delete", this);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_updateButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();

    layout->addWidget(m_filterEdit);
    layout->addWidget(m_table);
    layout->addLayout(buttonLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &PatientsWidget::updateActionStates);
    connect(m_table, &QTableWidget::itemDoubleClicked, this, &PatientsWidget::openPatientVisits);
    connect(m_addButton, &QPushButton::clicked, this, &PatientsWidget::handleAdd);
    connect(m_updateButton, &QPushButton::clicked, this, &PatientsWidget::handleUpdate);
    connect(m_deleteButton, &QPushButton::clicked, this, &PatientsWidget::handleDelete);
    connect(m_filterEdit, &QLineEdit::textChanged, this, &PatientsWidget::applyFilter);

    loadPatients();
    updateActionStates();
}

void PatientsWidget::loadPatients() {
    m_allPatients = m_controller.listPatients();
    applyFilter();
}

void PatientsWidget::applyFilter() {
    const QString filter = m_filterEdit->text().trimmed();

    QVector<Patient> filtered;
    if (filter.isEmpty()) {
        filtered = m_allPatients;
    } else {
        for (const Patient& patient : m_allPatients) {
            if (matchesFilter(patient, filter)) {
                filtered.append(patient);
            }
        }
    }

    m_table->setRowCount(filtered.size());
    for (int row = 0; row < filtered.size(); ++row) {
        const Patient& patient = filtered.at(row);

        auto* idItem = new QTableWidgetItem(QString::number(patient.id()));
        auto* firstItem = new QTableWidgetItem(patient.firstName());
        auto* lastItem = new QTableWidgetItem(patient.lastName());
        auto* birthItem = new QTableWidgetItem(patient.birthdate().toString("yyyy-MM-dd"));
        auto* phoneItem = new QTableWidgetItem(patient.phone());

        m_table->setItem(row, 0, idItem);
        m_table->setItem(row, 1, firstItem);
        m_table->setItem(row, 2, lastItem);
        m_table->setItem(row, 3, birthItem);
        m_table->setItem(row, 4, phoneItem);
    }

    updateActionStates();
}

void PatientsWidget::handleAdd() {
    PatientDialog dialog(this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Patient patient = dialog.patient();

    int newId = 0;
    if (!m_controller.createPatient(patient, newId)) {
        QMessageBox::warning(this, "Create failed", "Unable to create patient.");
        return;
    }

    loadPatients();
    QMessageBox::information(this, "Patient created", "Patient was added successfully.");
}

void PatientsWidget::handleUpdate() {
    const int id = selectedPatientId();
    if (id <= 0) {
        QMessageBox::information(this, "Update", "Select a patient to update.");
        return;
    }

    Patient existing;
    if (!m_controller.findPatient(id, existing)) {
        QMessageBox::warning(this, "Update failed", "Unable to load patient.");
        return;
    }

    PatientDialog dialog(this);
    dialog.setPatient(existing);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Patient patient = dialog.patient();

    if (!m_controller.updatePatient(patient)) {
        QMessageBox::warning(this, "Update failed", "Unable to update patient.");
        return;
    }

    loadPatients();
    QMessageBox::information(this, "Patient updated", "Patient was updated successfully.");
}

void PatientsWidget::handleDelete() {
    const int id = selectedPatientId();
    if (id <= 0) {
        QMessageBox::information(this, "Delete", "Select a patient to delete.");
        return;
    }

    const auto confirm = QMessageBox::question(this, "Delete", "Delete selected patient?");
    if (confirm != QMessageBox::Yes) {
        return;
    }

    if (!m_controller.removePatient(id)) {
        QMessageBox::warning(this, "Delete failed", "Unable to delete patient.");
        return;
    }

    loadPatients();
    QMessageBox::information(this, "Patient deleted", "Patient was deleted successfully.");
}

void PatientsWidget::openPatientVisits() {
    const int id = selectedPatientId();
    if (id <= 0) {
        return;
    }

    Patient patient;
    if (!m_controller.findPatient(id, patient)) {
        return;
    }

    const QString patientName = QString("%1 %2").arg(patient.firstName(), patient.lastName());
    PatientVisitsDialog dialog(m_visitController,
                               m_doctorController,
                               m_controller,
                               m_lookupRepository,
                               m_session,
                               id,
                               patientName,
                               this);
    dialog.exec();
}

int PatientsWidget::selectedPatientId() const {
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

bool PatientsWidget::matchesFilter(const Patient& patient, const QString& filter) const {
    const QString composite = QString("%1 %2 %3 %4")
        .arg(patient.firstName(), patient.lastName(), patient.birthdate().toString("yyyy-MM-dd"), patient.phone());
    return composite.contains(filter, Qt::CaseInsensitive);
}

void PatientsWidget::updateActionStates() {
    const bool hasSelection = selectedPatientId() > 0;
    m_updateButton->setEnabled(hasSelection);
    m_deleteButton->setEnabled(hasSelection);
}
