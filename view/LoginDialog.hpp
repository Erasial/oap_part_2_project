#pragma once

#include <QDialog>

class AuthService;

class QLineEdit;

class LoginDialog : public QDialog {
    Q_OBJECT

public:
    explicit LoginDialog(AuthService& authService, QWidget* parent = nullptr);

private slots:
    void handleLogin();

private:
    AuthService& m_authService;
    QLineEdit* m_usernameEdit = nullptr;
    QLineEdit* m_passwordEdit = nullptr;
};
