#include <QApplication>
#include <QCoreApplication>
#include <QMessageBox>

#include "controller/AuthService.hpp"
#include "controller/GcryptPasswordHasher.hpp"
#include "controller/DatabaseManager.hpp"
#include "controller/DoctorController.hpp"
#include "controller/DoctorRepository.hpp"
#include "controller/LookupRepository.hpp"
#include "controller/PatientController.hpp"
#include "controller/PatientRepository.hpp"
#include "controller/SessionContext.hpp"
#include "controller/UserRepository.hpp"
#include "controller/VisitController.hpp"
#include "controller/VisitRepository.hpp"
#include "view/LoginDialog.hpp"
#include "view/MainWindow.hpp"

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);

    const QString rootDir = QCoreApplication::applicationDirPath();
    const QString databasePath = rootDir + "/../database/db.sqlite";
    const QString schemaPath = rootDir + "/../database/init.sql";
    const QString seedDirPath = rootDir + "/../database/seed";

    if (!DatabaseManager::instance().initialize(databasePath, schemaPath, seedDirPath)) {
        return 1;
    }

    QString bootstrapError;
    if (!DatabaseManager::instance().verifyBootstrap(&bootstrapError)) {
        QMessageBox::critical(nullptr, "Startup error", bootstrapError);
        return 1;
    }

    QSqlDatabase db = DatabaseManager::instance().database();

    UserRepository userRepository(db);
    DoctorRepository doctorRepository(db);
    PatientRepository patientRepository(db);
    VisitRepository visitRepository(db);
    LookupRepository lookupRepository(db);
    SessionContext session;
    GcryptPasswordHasher hasher;
    AuthService authService(userRepository, hasher, session);
    DoctorController doctorController(doctorRepository, session);
    PatientController patientController(patientRepository, session);
    VisitController visitController(visitRepository, session);

    LoginDialog loginDialog(authService);
    if (loginDialog.exec() != QDialog::Accepted) {
        return 0;
    }

    MainWindow mainWindow(session, patientController, visitController, doctorController, lookupRepository, hasher);
    mainWindow.show();

    return app.exec();
}
