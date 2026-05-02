#pragma once

#include <QWidget>

#include "controller/DoctorController.hpp"
#include "controller/PasswordHasher.hpp"

class QLineEdit;
class QTableWidget;

class DoctorsWidget : public QWidget {
    Q_OBJECT

public:
    DoctorsWidget(DoctorController& controller, const PasswordHasher& hasher, QWidget* parent = nullptr);

private slots:
    void loadDoctors();
    void onSelectionChanged();
    void handleAdd();
    void handleUpdate();
    void handleDelete();
    void clearForm();

private:
    Doctor buildDoctorFromForm(int id) const;
    void setFormFromDoctor(const Doctor& doctor);
    int selectedDoctorId() const;
    bool validateForm(bool requirePassword, QString& errorMessage) const;

    DoctorController& m_controller;
    const PasswordHasher& m_hasher;
    QString m_currentPasswordHash;
    QTableWidget* m_table = nullptr;
    QLineEdit* m_fullNameEdit = nullptr;
    QLineEdit* m_usernameEdit = nullptr;
    QLineEdit* m_passwordEdit = nullptr;
    QLineEdit* m_specializationEdit = nullptr;
};
