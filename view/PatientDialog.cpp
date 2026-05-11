#include "PatientDialog.hpp"

#include <QDate>
#include <QDateEdit>
#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QVBoxLayout>

PatientDialog::PatientDialog(QWidget* parent)
    : QDialog(parent) {
    setWindowTitle("Patient");
    setModal(true);

    m_firstNameEdit = new QLineEdit(this);
    m_lastNameEdit = new QLineEdit(this);
    m_birthdateEdit = new QDateEdit(this);
    m_birthdateEdit->setCalendarPopup(true);
    m_birthdateEdit->setDisplayFormat("yyyy-MM-dd");
    m_birthdateEdit->setDate(QDate::currentDate());
    m_phoneEdit = new QLineEdit(this);

    auto* formLayout = new QFormLayout();
    formLayout->addRow("First name", m_firstNameEdit);
    formLayout->addRow("Last name", m_lastNameEdit);
    formLayout->addRow("Birthdate", m_birthdateEdit);
    formLayout->addRow("Phone", m_phoneEdit);

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
}

void PatientDialog::setPatient(const Patient& patient) {
    m_id = patient.id();
    m_firstNameEdit->setText(patient.firstName());
    m_lastNameEdit->setText(patient.lastName());
    m_birthdateEdit->setDate(patient.birthdate());
    m_phoneEdit->setText(patient.phone());
}

Patient PatientDialog::patient() const {
    Patient patient;
    patient.setId(m_id);
    patient.setFirstName(m_firstNameEdit->text().trimmed());
    patient.setLastName(m_lastNameEdit->text().trimmed());
    patient.setFullName(patient.firstName() + " " + patient.lastName());
    patient.setBirthdate(m_birthdateEdit->date());
    patient.setPhone(m_phoneEdit->text().trimmed());
    return patient;
}

bool PatientDialog::validate(QString& errorMessage) const {
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
