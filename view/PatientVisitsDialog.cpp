#include "PatientVisitsDialog.hpp"

#include "VisitDialog.hpp"

#include <QHash>
#include <QHeaderView>
#include <QMessageBox>
#include <QTableWidget>
#include <QVBoxLayout>

PatientVisitsDialog::PatientVisitsDialog(VisitController& visitController,
                                                                                 DoctorController& doctorController,
                                                                                 PatientController& patientController,
                                                                                 LookupRepository& lookupRepository,
                                                                                 SessionContext& session,
                                                                                 int patientId,
                                                                                 const QString& patientName,
                                                                                 QWidget* parent)
        : QDialog(parent),
            m_visitController(visitController),
            m_doctorController(doctorController),
            m_patientController(patientController),
            m_lookupRepository(lookupRepository),
            m_session(session),
            m_patientId(patientId) {
    setWindowTitle("Visits: " + patientName);
    setModal(true);

    m_table = new QTableWidget(this);
        m_table->setColumnCount(5);
        m_table->setHorizontalHeaderLabels({"ID", "Date", "Doctor", "Duration", "Notes"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
        m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
        m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::ResizeToContents);

    auto* layout = new QVBoxLayout(this);
    layout->addWidget(m_table);

    connect(m_table, &QTableWidget::itemDoubleClicked, this, &PatientVisitsDialog::openVisitEditor);

    loadVisits();
}

void PatientVisitsDialog::loadVisits() {
    const QVector<Visit> visits = m_visitController.listByPatient(m_patientId);
    const QVector<Doctor> doctors = m_doctorController.listDoctors();
    QHash<int, QString> doctorNames;
    for (const Doctor& doctor : doctors) {
        doctorNames.insert(doctor.id(), doctor.fullName());
    }

    m_table->setRowCount(visits.size());
    for (int row = 0; row < visits.size(); ++row) {
        const Visit& visit = visits.at(row);
        auto* idItem = new QTableWidgetItem(QString::number(visit.id()));
        auto* dateItem = new QTableWidgetItem(visit.visitDate().toString("yyyy-MM-dd HH:mm"));
        const QString doctorName = doctorNames.value(visit.doctorId(), QString::number(visit.doctorId()));
        auto* doctorItem = new QTableWidgetItem(doctorName);
        auto* durationItem = new QTableWidgetItem(QString::number(visit.durationMinutes()));
        auto* notesItem = new QTableWidgetItem(visit.notes());

        m_table->setItem(row, 0, idItem);
        m_table->setItem(row, 1, dateItem);
        m_table->setItem(row, 2, doctorItem);
        m_table->setItem(row, 3, durationItem);
        m_table->setItem(row, 4, notesItem);
    }
}

int PatientVisitsDialog::selectedVisitId() const {
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

void PatientVisitsDialog::openVisitEditor() {
    const int visitId = selectedVisitId();
    if (visitId <= 0) {
        return;
    }

    Visit existing;
    if (!m_visitController.findVisit(visitId, existing)) {
        QMessageBox::warning(this, "Update failed", "Unable to load visit.");
        return;
    }

    if (m_session.isDoctor() && existing.doctorId() != m_session.currentUser().id()) {
        QMessageBox::information(this, "Update", "You can only edit your own visits.");
        return;
    }

    VisitDialog dialog(m_patientController, m_doctorController, m_lookupRepository, m_session, this);
    dialog.setVisit(existing);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Visit updated = dialog.visit();
    if (!m_visitController.updateVisit(updated)) {
        QMessageBox::warning(this, "Update failed", "Unable to update visit.");
        return;
    }

    loadVisits();
    QMessageBox::information(this, "Visit updated", "Visit was updated successfully.");
}
