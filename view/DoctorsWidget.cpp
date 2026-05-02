#include "DoctorsWidget.hpp"

#include <QFormLayout>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

DoctorsWidget::DoctorsWidget(DoctorController& controller, const PasswordHasher& hasher, QWidget* parent)
        : QWidget(parent),
            m_controller(controller),
            m_hasher(hasher) {
    auto* layout = new QVBoxLayout(this);

    m_table = new QTableWidget(this);
    m_table->setColumnCount(4);
    m_table->setHorizontalHeaderLabels({"ID", "Full name", "Username", "Specialization"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::Stretch);

    auto* formLayout = new QFormLayout();
    m_fullNameEdit = new QLineEdit(this);
    m_usernameEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);
    m_specializationEdit = new QLineEdit(this);

    formLayout->addRow("Full name", m_fullNameEdit);
    formLayout->addRow("Username", m_usernameEdit);
    formLayout->addRow("Password", m_passwordEdit);
    formLayout->addRow("Specialization", m_specializationEdit);

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

    layout->addWidget(m_table);
    layout->addLayout(formLayout);
    layout->addLayout(buttonLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &DoctorsWidget::onSelectionChanged);
    connect(addButton, &QPushButton::clicked, this, &DoctorsWidget::handleAdd);
    connect(updateButton, &QPushButton::clicked, this, &DoctorsWidget::handleUpdate);
    connect(deleteButton, &QPushButton::clicked, this, &DoctorsWidget::handleDelete);
    connect(clearButton, &QPushButton::clicked, this, &DoctorsWidget::clearForm);

    loadDoctors();
}

void DoctorsWidget::loadDoctors() {
    const QVector<Doctor> doctors = m_controller.listDoctors();

    m_table->setRowCount(doctors.size());
    for (int row = 0; row < doctors.size(); ++row) {
        const Doctor& doctor = doctors.at(row);

        auto* idItem = new QTableWidgetItem(QString::number(doctor.id()));
        auto* nameItem = new QTableWidgetItem(doctor.fullName());
        auto* usernameItem = new QTableWidgetItem(doctor.username());
        auto* specItem = new QTableWidgetItem(doctor.specialization());
        m_table->setItem(row, 0, idItem);
        m_table->setItem(row, 1, nameItem);
        m_table->setItem(row, 2, usernameItem);
        m_table->setItem(row, 3, specItem);
    }
}

void DoctorsWidget::onSelectionChanged() {
    const int id = selectedDoctorId();
    if (id <= 0) {
        return;
    }

    Doctor doctor;
    if (!m_controller.findDoctor(id, doctor)) {
        return;
    }

    setFormFromDoctor(doctor);
}

void DoctorsWidget::handleAdd() {
    QString errorMessage;
    if (!validateForm(true, errorMessage)) {
        QMessageBox::warning(this, "Invalid data", errorMessage);
        return;
    }

    Doctor doctor = buildDoctorFromForm(0);
    int newId = 0;
    if (!m_controller.createDoctor(doctor, newId)) {
        QMessageBox::warning(this, "Create failed", "Unable to create doctor.");
        return;
    }

    loadDoctors();
    clearForm();
}

void DoctorsWidget::handleUpdate() {
    const int id = selectedDoctorId();
    if (id <= 0) {
        QMessageBox::information(this, "Update", "Select a doctor to update.");
        return;
    }

    QString errorMessage;
    if (!validateForm(false, errorMessage)) {
        QMessageBox::warning(this, "Invalid data", errorMessage);
        return;
    }

    Doctor doctor = buildDoctorFromForm(id);
    if (!m_controller.updateDoctor(doctor)) {
        QMessageBox::warning(this, "Update failed", "Unable to update doctor.");
        return;
    }

    loadDoctors();
}

void DoctorsWidget::handleDelete() {
    const int id = selectedDoctorId();
    if (id <= 0) {
        QMessageBox::information(this, "Delete", "Select a doctor to delete.");
        return;
    }

    const auto confirm = QMessageBox::question(this, "Delete", "Delete selected doctor?");
    if (confirm != QMessageBox::Yes) {
        return;
    }

    if (!m_controller.removeDoctor(id)) {
        QMessageBox::warning(this, "Delete failed", "Unable to delete doctor.");
        return;
    }

    loadDoctors();
    clearForm();
}

void DoctorsWidget::clearForm() {
    m_fullNameEdit->clear();
    m_usernameEdit->clear();
    m_passwordEdit->clear();
    m_specializationEdit->clear();
    m_currentPasswordHash.clear();
    m_table->clearSelection();
}

Doctor DoctorsWidget::buildDoctorFromForm(int id) const {
    Doctor doctor;
    doctor.setId(id);
    doctor.setFullName(m_fullNameEdit->text().trimmed());
    doctor.setUsername(m_usernameEdit->text().trimmed());
    const QString rawPassword = m_passwordEdit->text();
    if (rawPassword.isEmpty()) {
        doctor.setPasswordHash(m_currentPasswordHash);
    } else {
        doctor.setPasswordHash(m_hasher.hash(rawPassword));
    }
    doctor.setSpecialization(m_specializationEdit->text().trimmed());

    return doctor;
}

void DoctorsWidget::setFormFromDoctor(const Doctor& doctor) {
    m_fullNameEdit->setText(doctor.fullName());
    m_usernameEdit->setText(doctor.username());
    m_currentPasswordHash = doctor.passwordHash();
    m_passwordEdit->clear();
    m_specializationEdit->setText(doctor.specialization());
}

int DoctorsWidget::selectedDoctorId() const {
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

bool DoctorsWidget::validateForm(bool requirePassword, QString& errorMessage) const {
    const QString fullName = m_fullNameEdit->text().trimmed();
    const QString username = m_usernameEdit->text().trimmed();
    const QString specialization = m_specializationEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (fullName.isEmpty() || username.isEmpty() || specialization.isEmpty()) {
        errorMessage = "Full name, username, and specialization are required.";
        return false;
    }

    if (requirePassword && password.isEmpty()) {
        errorMessage = "Password is required for new doctor accounts.";
        return false;
    }

    return true;
}
