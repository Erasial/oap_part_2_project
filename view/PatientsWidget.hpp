#pragma once

#include <QWidget>

#include "controller/PatientController.hpp"

class QDateEdit;
class QLineEdit;
class QTableWidget;

class PatientsWidget : public QWidget {
    Q_OBJECT

public:
    explicit PatientsWidget(PatientController& controller, QWidget* parent = nullptr);

private slots:
    void loadPatients();
    void applyFilter();
    void onSelectionChanged();
    void handleAdd();
    void handleUpdate();
    void handleDelete();
    void clearForm();

private:
    Patient buildPatientFromForm(int id) const;
    void setFormFromPatient(const Patient& patient);
    void setFormEnabled(bool enabled);
    int selectedPatientId() const;
    bool validateForm(QString& errorMessage) const;
    bool matchesFilter(const Patient& patient, const QString& filter) const;

    PatientController& m_controller;
    QTableWidget* m_table = nullptr;
    QLineEdit* m_filterEdit = nullptr;
    QLineEdit* m_firstNameEdit = nullptr;
    QLineEdit* m_lastNameEdit = nullptr;
    QDateEdit* m_birthdateEdit = nullptr;
    QLineEdit* m_phoneEdit = nullptr;
    QVector<Patient> m_allPatients;
};
