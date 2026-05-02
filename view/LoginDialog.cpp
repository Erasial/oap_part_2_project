#include "LoginDialog.hpp"

#include "controller/AuthService.hpp"

#include <QFormLayout>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>

LoginDialog::LoginDialog(AuthService& authService, QWidget* parent)
    : QDialog(parent),
      m_authService(authService) {
    setWindowTitle("Login");
    setModal(true);

    m_usernameEdit = new QLineEdit(this);
    m_passwordEdit = new QLineEdit(this);
    m_passwordEdit->setEchoMode(QLineEdit::Password);

    auto* formLayout = new QFormLayout();
    formLayout->addRow("Username", m_usernameEdit);
    formLayout->addRow("Password", m_passwordEdit);

    auto* loginButton = new QPushButton("Login", this);
    auto* cancelButton = new QPushButton("Cancel", this);

    auto* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    buttonLayout->addWidget(loginButton);
    buttonLayout->addWidget(cancelButton);

    auto* mainLayout = new QVBoxLayout();
    mainLayout->addLayout(formLayout);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);

    connect(loginButton, &QPushButton::clicked, this, &LoginDialog::handleLogin);
    connect(cancelButton, &QPushButton::clicked, this, &LoginDialog::reject);
}

void LoginDialog::handleLogin() {
    const QString username = m_usernameEdit->text().trimmed();
    const QString password = m_passwordEdit->text();

    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(this, "Login failed", "Enter username and password.");
        return;
    }

    if (!m_authService.login(username, password)) {
        QMessageBox::warning(this, "Login failed", "Invalid username or password.");
        return;
    }

    accept();
}
