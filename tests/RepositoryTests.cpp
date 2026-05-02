#include <QtTest/QtTest>

#include "controller/AuthService.hpp"
#include "controller/GcryptPasswordHasher.hpp"
#include "controller/DoctorController.hpp"
#include "controller/DoctorRepository.hpp"
#include "controller/PatientController.hpp"
#include "controller/PatientRepository.hpp"
#include "controller/SessionContext.hpp"
#include "controller/UserRepository.hpp"
#include "controller/VisitController.hpp"
#include "controller/VisitRepository.hpp"

#include <QFile>
#include <QSqlQuery>
#include <QUuid>

namespace {
QString readFileContents(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return QString();
    }
    return QString::fromUtf8(file.readAll());
}

bool executeSqlScript(QSqlDatabase& db, const QString& sql) {
    QString current;
    for (const QChar ch : sql) {
        if (ch == ';') {
            const QString statement = current.trimmed();
            current.clear();
            if (statement.isEmpty()) {
                continue;
            }
            QSqlQuery query(db);
            if (!query.exec(statement)) {
                return false;
            }
        } else {
            current.append(ch);
        }
    }

    const QString trailing = current.trimmed();
    if (!trailing.isEmpty()) {
        QSqlQuery query(db);
        if (!query.exec(trailing)) {
            return false;
        }
    }

    return true;
}

QSqlDatabase createTestDatabase() {
    const QString connectionName = QUuid::createUuid().toString();
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE", connectionName);
    db.setDatabaseName(":memory:");
    if (!db.open()) {
        return QSqlDatabase();
    }

    QSqlQuery pragma(db);
    pragma.exec("PRAGMA foreign_keys = ON");

    const QString schemaPath = QString(PROJECT_ROOT) + "/database/init.sql";
    const QString schemaSql = readFileContents(schemaPath);
    if (schemaSql.isEmpty()) {
        return QSqlDatabase();
    }

    if (!executeSqlScript(db, schemaSql)) {
        return QSqlDatabase();
    }

    return db;
}

void seedLookup(QSqlDatabase& db) {
    QSqlQuery query(db);
    query.exec("INSERT INTO icd10_codes (code, name) VALUES ('A00', 'Test ICD')");
    query.exec("INSERT INTO icpc2_codes (code, name) VALUES ('A01', 'Test ICPC')");
    query.exec("INSERT INTO achi_codes (code, name) VALUES ('X01', 'Test ACHI')");
}
}

class RepositoryTests : public QObject {
    Q_OBJECT

private slots:
    void testAdminLogin();
    void testDoctorCrud();
    void testPatientVisitFlow();
    void testAccessRules();
    void testVisitControllerScoping();
};

void RepositoryTests::testAdminLogin() {
    QSqlDatabase db = createTestDatabase();
    QVERIFY(db.isOpen());

    UserRepository userRepository(db);
    SessionContext session;
    GcryptPasswordHasher hasher;
    AuthService authService(userRepository, hasher, session);

    QVERIFY(authService.login("admin", "admin123"));
    QVERIFY(session.isAdmin());
    authService.logout();
    QVERIFY(!session.isAuthenticated());

    QVERIFY(!authService.login("admin", "wrong"));
}

void RepositoryTests::testDoctorCrud() {
    QSqlDatabase db = createTestDatabase();
    QVERIFY(db.isOpen());

    DoctorRepository repository(db);
    GcryptPasswordHasher hasher;

    Doctor doctor;
    doctor.setFullName("Dr Test");
    doctor.setUsername("drtest");
    doctor.setPasswordHash(hasher.hash("password"));
    doctor.setSpecialization("Therapy");

    int newId = 0;
    QVERIFY(repository.create(doctor, newId));
    QVERIFY(newId > 0);

    Doctor loaded;
    QVERIFY(repository.findById(newId, loaded));
    QCOMPARE(loaded.fullName(), QString("Dr Test"));
    QCOMPARE(loaded.specialization(), QString("Therapy"));

    loaded.setSpecialization("Cardiology");
    QVERIFY(repository.update(loaded));

    Doctor updated;
    QVERIFY(repository.findById(newId, updated));
    QCOMPARE(updated.specialization(), QString("Cardiology"));

    QVERIFY(repository.remove(newId));
    Doctor removed;
    QVERIFY(!repository.findById(newId, removed));
}

void RepositoryTests::testPatientVisitFlow() {
    QSqlDatabase db = createTestDatabase();
    QVERIFY(db.isOpen());

    seedLookup(db);

    DoctorRepository doctorRepository(db);
    GcryptPasswordHasher hasher;

    Doctor doctor;
    doctor.setFullName("Dr Visit");
    doctor.setUsername("drvisit");
    doctor.setPasswordHash(hasher.hash("password"));
    doctor.setSpecialization("Surgery");

    int doctorId = 0;
    QVERIFY(doctorRepository.create(doctor, doctorId));
    QVERIFY(doctorId > 0);

    PatientRepository patientRepository(db);
    Patient patient;
    patient.setFirstName("Jane");
    patient.setLastName("Doe");
    patient.setFullName("Jane Doe");
    patient.setBirthdate(QDate(1990, 1, 1));
    patient.setPhone("123456");

    int patientId = 0;
    QVERIFY(patientRepository.create(patient, patientId));
    QVERIFY(patientId > 0);

    VisitRepository visitRepository(db);
    Visit visit;
    visit.setPatientId(patientId);
    visit.setDoctorId(doctorId);
    visit.setVisitDate(QDateTime::currentDateTime());
    visit.setDurationMinutes(30);
    visit.setNotes("Initial visit");

    visit.setDiagnosisIds({1});
    visit.setReasonIds({1});
    visit.setProcedureIds({1});

    int visitId = 0;
    QVERIFY(visitRepository.create(visit, visitId));
    QVERIFY(visitId > 0);

    Visit loaded;
    QVERIFY(visitRepository.findById(visitId, loaded));
    QCOMPARE(loaded.patientId(), patientId);
    QCOMPARE(loaded.doctorId(), doctorId);
    QCOMPARE(loaded.durationMinutes(), 30);
    QCOMPARE(loaded.notes(), QString("Initial visit"));
    QCOMPARE(loaded.diagnosisIds().size(), 1);
    QCOMPARE(loaded.reasonIds().size(), 1);
    QCOMPARE(loaded.procedureIds().size(), 1);
}

void RepositoryTests::testAccessRules() {
    QSqlDatabase db = createTestDatabase();
    QVERIFY(db.isOpen());

    seedLookup(db);

    DoctorRepository doctorRepository(db);
    PatientRepository patientRepository(db);
    VisitRepository visitRepository(db);

    SessionContext session;
    DoctorController doctorController(doctorRepository, session);
    PatientController patientController(patientRepository, session);
    VisitController visitController(visitRepository, session);

    GcryptPasswordHasher hasher;

    Doctor doctor1;
    doctor1.setFullName("Dr One");
    doctor1.setUsername("drone");
    doctor1.setPasswordHash(hasher.hash("pass1"));
    doctor1.setSpecialization("General");

    int doctor1Id = 0;
    QVERIFY(doctorRepository.create(doctor1, doctor1Id));

    Doctor doctor2;
    doctor2.setFullName("Dr Two");
    doctor2.setUsername("drtwo");
    doctor2.setPasswordHash(hasher.hash("pass2"));
    doctor2.setSpecialization("Cardiology");

    int doctor2Id = 0;
    QVERIFY(doctorRepository.create(doctor2, doctor2Id));

    Patient patient;
    patient.setFirstName("Tom");
    patient.setLastName("Smith");
    patient.setFullName("Tom Smith");
    patient.setBirthdate(QDate(1985, 5, 5));
    patient.setPhone("555");

    int patientId = 0;
    QVERIFY(patientRepository.create(patient, patientId));

    Visit visit;
    visit.setPatientId(patientId);
    visit.setDoctorId(doctor1Id);
    visit.setVisitDate(QDateTime::currentDateTime());
    visit.setDurationMinutes(20);
    visit.setNotes("Follow-up");
    visit.setDiagnosisIds({1});
    visit.setReasonIds({1});
    visit.setProcedureIds({1});

    int visitId = 0;
    QVERIFY(visitRepository.create(visit, visitId));

    User doctorUser;
    doctorUser.setId(doctor2Id);
    doctorUser.setFullName("Dr Two");
    doctorUser.setRole(RoleType::Doctor);
    session.setCurrentUser(doctorUser);

    Visit loaded;
    QVERIFY(visitRepository.findById(visitId, loaded));
    loaded.setNotes("Doctor two edit attempt");
    QVERIFY(!visitController.updateVisit(loaded));

    User adminUser;
    adminUser.setId(1);
    adminUser.setFullName("Admin");
    adminUser.setRole(RoleType::Admin);
    session.setCurrentUser(adminUser);

    loaded.setNotes("Admin edit");
    QVERIFY(visitController.updateVisit(loaded));

    Doctor newDoctor;
    newDoctor.setFullName("Dr Three");
    newDoctor.setUsername("drthree");
    newDoctor.setPasswordHash(hasher.hash("pass3"));
    newDoctor.setSpecialization("Neuro");

    int newDoctorId = 0;
    QVERIFY(doctorController.createDoctor(newDoctor, newDoctorId));

    session.setCurrentUser(doctorUser);
    int deniedDoctorId = 0;
    QVERIFY(!doctorController.createDoctor(newDoctor, deniedDoctorId));

    int newPatientId = 0;
    QVERIFY(patientController.createPatient(patient, newPatientId));
}

void RepositoryTests::testVisitControllerScoping() {
    QSqlDatabase db = createTestDatabase();
    QVERIFY(db.isOpen());

    seedLookup(db);

    DoctorRepository doctorRepository(db);
    PatientRepository patientRepository(db);
    VisitRepository visitRepository(db);
    SessionContext session;
    VisitController visitController(visitRepository, session);

    GcryptPasswordHasher hasher;

    Doctor doctor;
    doctor.setFullName("Dr Scope");
    doctor.setUsername("drscope");
    doctor.setPasswordHash(hasher.hash("pass"));
    doctor.setSpecialization("General");

    int doctorId = 0;
    QVERIFY(doctorRepository.create(doctor, doctorId));

    Patient patient;
    patient.setFirstName("Ana");
    patient.setLastName("Scope");
    patient.setFullName("Ana Scope");
    patient.setBirthdate(QDate(1992, 2, 2));
    patient.setPhone("777");

    int patientId = 0;
    QVERIFY(patientRepository.create(patient, patientId));

    Visit visit;
    visit.setPatientId(patientId);
    visit.setDoctorId(doctorId);
    visit.setVisitDate(QDateTime::currentDateTime());
    visit.setDurationMinutes(25);
    visit.setNotes("Scoped");
    visit.setDiagnosisIds({1});
    visit.setReasonIds({1});
    visit.setProcedureIds({1});

    int visitId = 0;
    QVERIFY(visitRepository.create(visit, visitId));

    User doctorUser;
    doctorUser.setId(doctorId);
    doctorUser.setFullName("Dr Scope");
    doctorUser.setRole(RoleType::Doctor);
    session.setCurrentUser(doctorUser);

    const QVector<Visit> ownVisits = visitController.listOwnVisits();
    QCOMPARE(ownVisits.size(), 1);
    QCOMPARE(ownVisits.first().id(), visitId);
}

QTEST_MAIN(RepositoryTests)
#include "RepositoryTests.moc"
