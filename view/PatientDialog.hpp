#pragma once

#include <QDialog>

#include "model/Patient.hpp"

class QDateEdit;
class QLineEdit;

class PatientDialog : public QDialog {
    Q_OBJECT

public:
    explicit PatientDialog(QWidget* parent = nullptr);

    void setPatient(const Patient& patient);
    Patient patient() const;

private:
    bool validate(QString& errorMessage) const;

    int m_id = 0;
    QLineEdit* m_firstNameEdit = nullptr;
    QLineEdit* m_lastNameEdit = nullptr;
    QDateEdit* m_birthdateEdit = nullptr;
    QLineEdit* m_phoneEdit = nullptr;
};
