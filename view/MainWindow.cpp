#include "MainWindow.hpp"

#include "controller/DoctorController.hpp"
#include "controller/LookupRepository.hpp"
#include "controller/PasswordHasher.hpp"
#include "controller/PatientController.hpp"
#include "controller/SessionContext.hpp"
#include "controller/VisitController.hpp"
#include "view/DoctorsWidget.hpp"
#include "view/PatientsWidget.hpp"
#include "view/VisitsWidget.hpp"

#include <QStatusBar>
#include <QTabWidget>
#include <QMenuBar>
#include <QAction>

MainWindow::MainWindow(SessionContext& session,
                       PatientController& patientController,
                       VisitController& visitController,
                       DoctorController& doctorController,
                       LookupRepository& lookupRepository,
                       const PasswordHasher& hasher,
                       QWidget* parent)
    : QMainWindow(parent),
      m_session(session),
      m_patientController(patientController),
      m_visitController(visitController),
      m_doctorController(doctorController),
      m_lookupRepository(lookupRepository),
      m_hasher(hasher) {
    setWindowTitle("MedLogApp");
    resize(1024, 768);

    auto* sessionMenu = menuBar()->addMenu("Session");
    auto* logoutAction = new QAction("Logout", this);
    sessionMenu->addAction(logoutAction);

    auto* tabs = new QTabWidget(this);
    tabs->addTab(new PatientsWidget(m_patientController,
                    m_visitController,
                    m_doctorController,
                    m_lookupRepository,
                    m_session,
                    tabs),
           "Patients");
    tabs->addTab(new VisitsWidget(m_visitController, m_patientController, m_doctorController, m_lookupRepository, m_session, tabs), "Visits");
    if (m_session.isAdmin()) {
      tabs->addTab(new DoctorsWidget(m_doctorController, m_hasher, tabs), "Doctors");
    }

    setCentralWidget(tabs);

    const QString roleLabel = m_session.isAdmin() ? "Role: Admin" : "Role: Doctor";
    statusBar()->showMessage(roleLabel);

    connect(logoutAction, &QAction::triggered, this, [this]() {
      emit logoutRequested();
      close();
    });
}
