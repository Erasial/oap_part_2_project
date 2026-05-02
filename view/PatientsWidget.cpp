#include "PatientsWidget.hpp"

#include <QDateEdit>
#include <QFormLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

PatientsWidget::PatientsWidget(PatientController& controller, QWidget* parent)
    : QWidget(parent),
      m_controller(controller) {
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

    auto* formLayout = new QFormLayout();
    m_firstNameEdit = new QLineEdit(this);
    m_lastNameEdit = new QLineEdit(this);
    m_birthdateEdit = new QDateEdit(this);
    m_birthdateEdit->setCalendarPopup(true);
    m_birthdateEdit->setDisplayFormat("yyyy-MM-dd");
    m_phoneEdit = new QLineEdit(this);

    formLayout->addRow("First name", m_firstNameEdit);
    formLayout->addRow("Last name", m_lastNameEdit);
    formLayout->addRow("Birthdate", m_birthdateEdit);
    formLayout->addRow("Phone", m_phoneEdit);

    auto* buttonLayout = new QHBoxLayout();
    auto* addButton = new QPushButton("Add", this);
    auto* updateButton = new QPushButton("Update", this);
    auto* deleteButton = new QPushButton("Delete", this);
    auto* clearButton = new QPushButton("Clear", this);

    buttonLayout->addWidget(addButton);
    buttonLayout->addWidget(updateButton);
    buttonLayout->addWidget(deleteButton);
    buttonLayout->addStretch();
    buttonLayout->addWidget(clearButton);

    layout->addWidget(m_filterEdit);
    layout->addWidget(m_table);
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &PatientsWidget::onSelectionChanged);
    connect(addButton, &QPushButton::clicked, this, &PatientsWidget::handleAdd);
    connect(updateButton, &QPushButton::clicked, this, &PatientsWidget::handleUpdate);
    connect(deleteButton, &QPushButton::clicked, this, &PatientsWidget::handleDelete);
    connect(clearButton, &QPushButton::clicked, this, &PatientsWidget::clearForm);
    connect(m_filterEdit, &QLineEdit::textChanged, this, &PatientsWidget::applyFilter);

    loadPatients();
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
}

void PatientsWidget::onSelectionChanged() {
    const int id = selectedPatientId();
    if (id <= 0) {
        return;
    }

    Patient patient;
    if (!m_controller.findPatient(id, patient)) {
        return;
    }

    setFormFromPatient(patient);
}

void PatientsWidget::handleAdd() {
    QString errorMessage;
    if (!validateForm(errorMessage)) {
        QMessageBox::warning(this, "Invalid data", errorMessage);
        return;
    }

    Patient patient = buildPatientFromForm(0);

    int newId = 0;
    if (!m_controller.createPatient(patient, newId)) {
        QMessageBox::warning(this, "Create failed", "Unable to create patient.");
        return;
    }

    loadPatients();
    clearForm();
}

void PatientsWidget::handleUpdate() {
    const int id = selectedPatientId();
    if (id <= 0) {
        QMessageBox::information(this, "Update", "Select a patient to update.");
        return;
    }

    QString errorMessage;
    if (!validateForm(errorMessage)) {
        QMessageBox::warning(this, "Invalid data", errorMessage);
        return;
    }

    Patient patient = buildPatientFromForm(id);

    if (!m_controller.updatePatient(patient)) {
        QMessageBox::warning(this, "Update failed", "Unable to update patient.");
        return;
    }

    loadPatients();
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
    clearForm();
}

void PatientsWidget::clearForm() {
    m_firstNameEdit->clear();
    m_lastNameEdit->clear();
    m_birthdateEdit->setDate(QDate::currentDate());
    m_phoneEdit->clear();
    m_table->clearSelection();
}

Patient PatientsWidget::buildPatientFromForm(int id) const {
    const QString firstName = m_firstNameEdit->text().trimmed();
    const QString lastName = m_lastNameEdit->text().trimmed();

    Patient patient;
    patient.setId(id);
    patient.setFirstName(firstName);
    patient.setLastName(lastName);
    patient.setFullName(firstName + " " + lastName);
    patient.setBirthdate(m_birthdateEdit->date());
    patient.setPhone(m_phoneEdit->text().trimmed());

    return patient;
}

void PatientsWidget::setFormFromPatient(const Patient& patient) {
    m_firstNameEdit->setText(patient.firstName());
    m_lastNameEdit->setText(patient.lastName());
    m_birthdateEdit->setDate(patient.birthdate());
    m_phoneEdit->setText(patient.phone());
}

void PatientsWidget::setFormEnabled(bool enabled) {
    m_firstNameEdit->setEnabled(enabled);
    m_lastNameEdit->setEnabled(enabled);
    m_birthdateEdit->setEnabled(enabled);
    m_phoneEdit->setEnabled(enabled);
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

bool PatientsWidget::validateForm(QString& errorMessage) const {
    const QString firstName = m_firstNameEdit->text().trimmed();
    const QString lastName = m_lastNameEdit->text().trimmed();
    const QDate birthdate = m_birthdateEdit->date();

    if (firstName.isEmpty() || lastName.isEmpty()) {
        errorMessage = "First and last name are required.";
        return false;
    }

    if (!birthdate.isValid()) {
        errorMessage = "Birthdate is required.";
        return false;
    }

    if (birthdate > QDate::currentDate()) {
        errorMessage = "Birthdate cannot be in the future.";
        return false;
    }

    return true;
}

bool PatientsWidget::matchesFilter(const Patient& patient, const QString& filter) const {
    const QString composite = QString("%1 %2 %3 %4")
        .arg(patient.firstName(), patient.lastName(), patient.birthdate().toString("yyyy-MM-dd"), patient.phone());
    return composite.contains(filter, Qt::CaseInsensitive);
}
