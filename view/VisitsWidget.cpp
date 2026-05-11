#include "VisitsWidget.hpp"

#include "VisitDialog.hpp"

#include <QDialog>
#include <QCheckBox>
#include <QHeaderView>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include <QTableWidget>
#include <QVBoxLayout>

VisitsWidget::VisitsWidget(VisitController& visitController,
                           PatientController& patientController,
                           DoctorController& doctorController,
                           LookupRepository& lookupRepository,
                           SessionContext& session,
                           QWidget* parent)
    : QWidget(parent),
      m_visitController(visitController),
      m_patientController(patientController),
      m_doctorController(doctorController),
      m_lookupRepository(lookupRepository),
      m_session(session) {
        setMinimumSize(980, 700);
    auto* layout = new QVBoxLayout(this);

    m_visitFilter = new QLineEdit(this);
    m_visitFilter->setPlaceholderText("Search visits...");

    m_myVisitsOnly = new QCheckBox("Only my visits", this);
    if (!m_session.isDoctor()) {
        m_myVisitsOnly->setChecked(false);
        m_myVisitsOnly->setEnabled(false);
    }

    m_table = new QTableWidget(this);
    m_table->setColumnCount(6);
    m_table->setHorizontalHeaderLabels({"ID", "Patient", "Doctor", "Date", "Duration", "Notes"});
    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->setSelectionMode(QAbstractItemView::SingleSelection);
    m_table->setEditTriggers(QAbstractItemView::NoEditTriggers);
    m_table->setColumnHidden(0, true);
    m_table->horizontalHeader()->setStretchLastSection(true);
    m_table->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(2, QHeaderView::Stretch);
    m_table->horizontalHeader()->setSectionResizeMode(3, QHeaderView::ResizeToContents);

    auto* buttonLayout = new QHBoxLayout();
    m_addButton = new QPushButton("Add", this);
    m_updateButton = new QPushButton("Edit", this);
    m_deleteButton = new QPushButton("Delete", this);

    buttonLayout->addWidget(m_addButton);
    buttonLayout->addWidget(m_updateButton);
    buttonLayout->addWidget(m_deleteButton);
    buttonLayout->addStretch();

    layout->addWidget(m_visitFilter);
    layout->addWidget(m_myVisitsOnly);
    layout->addWidget(m_table);
    layout->addLayout(buttonLayout);

    connect(m_table, &QTableWidget::itemSelectionChanged, this, &VisitsWidget::updateActionStates);
    connect(m_table, &QTableWidget::itemDoubleClicked, this, &VisitsWidget::openVisitEditor);
    connect(m_addButton, &QPushButton::clicked, this, &VisitsWidget::handleAdd);
    connect(m_updateButton, &QPushButton::clicked, this, &VisitsWidget::handleUpdate);
    connect(m_deleteButton, &QPushButton::clicked, this, &VisitsWidget::handleDelete);
    connect(m_visitFilter, &QLineEdit::textChanged, this, &VisitsWidget::applyVisitFilter);
    connect(m_myVisitsOnly, &QCheckBox::toggled, this, &VisitsWidget::applyVisitFilter);

    loadVisits();
    updateActionStates();
}

void VisitsWidget::loadVisits() {
    loadPatients();
    loadDoctors();
    m_allVisits = m_visitController.listVisits();
    applyVisitFilter();
}

void VisitsWidget::applyVisitFilter() {
    const QString filter = m_visitFilter->text().trimmed();
    QVector<Visit> filtered;

    const bool onlyMine = m_myVisitsOnly->isEnabled() && m_myVisitsOnly->isChecked();
    for (const Visit& visit : m_allVisits) {
        if (onlyMine && visit.doctorId() != m_session.currentUser().id()) {
            continue;
        }
        if (filter.isEmpty() || matchesVisitFilter(visit, filter)) {
            filtered.append(visit);
        }
    }

    m_table->setRowCount(filtered.size());
    for (int row = 0; row < filtered.size(); ++row) {
        const Visit& visit = filtered.at(row);

        auto* idItem = new QTableWidgetItem(QString::number(visit.id()));
        const QString patientName = m_patientNames.value(visit.patientId(), QString("%1").arg(visit.patientId()));
        const QString doctorName = m_doctorNames.value(visit.doctorId(), QString("%1").arg(visit.doctorId()));
        auto* patientItem = new QTableWidgetItem(patientName);
        auto* doctorItem = new QTableWidgetItem(doctorName);
        auto* dateItem = new QTableWidgetItem(visit.visitDate().toString("yyyy-MM-dd HH:mm"));
        auto* durationItem = new QTableWidgetItem(QString::number(visit.durationMinutes()));
        auto* notesItem = new QTableWidgetItem(visit.notes());

        m_table->setItem(row, 0, idItem);
        m_table->setItem(row, 1, patientItem);
        m_table->setItem(row, 2, doctorItem);
        m_table->setItem(row, 3, dateItem);
        m_table->setItem(row, 4, durationItem);
        m_table->setItem(row, 5, notesItem);
    }

    updateActionStates();
}

void VisitsWidget::handleAdd() {
    VisitDialog dialog(m_patientController, m_doctorController, m_lookupRepository, m_session, this);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Visit visit = dialog.visit();

    int newId = 0;
    if (!m_visitController.createVisit(visit, newId)) {
        QMessageBox::warning(this, "Create failed", "Unable to create visit.");
        return;
    }

    loadVisits();
    QMessageBox::information(this, "Visit created", "Visit was added successfully.");
}

void VisitsWidget::handleUpdate() {
    const int id = selectedVisitId();
    if (id <= 0) {
        QMessageBox::information(this, "Update", "Select a visit to update.");
        return;
    }

    Visit existing;
    if (!m_visitController.findVisit(id, existing)) {
        QMessageBox::warning(this, "Update failed", "Unable to load visit.");
        return;
    }

    VisitDialog dialog(m_patientController, m_doctorController, m_lookupRepository, m_session, this);
    dialog.setVisit(existing);
    if (dialog.exec() != QDialog::Accepted) {
        return;
    }

    Visit visit = dialog.visit();
    if (!m_visitController.updateVisit(visit)) {
        QMessageBox::warning(this, "Update failed", "Unable to update visit.");
        return;
    }

    loadVisits();
    QMessageBox::information(this, "Visit updated", "Visit was updated successfully.");
}

void VisitsWidget::handleDelete() {
    const int id = selectedVisitId();
    if (id <= 0) {
        QMessageBox::information(this, "Delete", "Select a visit to delete.");
        return;
    }

    const auto confirm = QMessageBox::question(this, "Delete", "Delete selected visit?");
    if (confirm != QMessageBox::Yes) {
        return;
    }

    if (!m_visitController.removeVisit(id)) {
        QMessageBox::warning(this, "Delete failed", "Unable to delete visit.");
        return;
    }

    loadVisits();
    QMessageBox::information(this, "Visit deleted", "Visit was deleted successfully.");
}

void VisitsWidget::openVisitEditor() {
    handleUpdate();
}

void VisitsWidget::loadPatients() {
    const QVector<Patient> patients = m_patientController.listPatients();
    m_patientNames.clear();

    for (const Patient& patient : patients) {
        const QString label = QString("%1 %2").arg(patient.firstName(), patient.lastName());
        const QString display = QString("%1 (ID:%2)").arg(label).arg(patient.id());
        m_patientNames.insert(patient.id(), display);
    }
}

void VisitsWidget::loadDoctors() {
    m_doctorNames.clear();

    if (m_session.isAdmin()) {
        const QVector<Doctor> doctors = m_doctorController.listDoctors();
        for (const Doctor& doctor : doctors) {
            const QString label = QString("%1 (%2)").arg(doctor.fullName(), doctor.specialization());
            const QString display = QString("%1 [ID:%2]").arg(label).arg(doctor.id());
            m_doctorNames.insert(doctor.id(), display);
        }
    } else {
        const QString display = QString("%1 [ID:%2]").arg(m_session.currentUser().fullName()).arg(m_session.currentUser().id());
        m_doctorNames.insert(m_session.currentUser().id(), display);
    }
}

int VisitsWidget::selectedVisitId() const {
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

void VisitsWidget::updateActionStates() {
    const int visitId = selectedVisitId();
    bool canEdit = false;

    if (visitId > 0) {
        Visit visit;
        if (m_visitController.findVisit(visitId, visit)) {
            if (m_session.isAdmin()) {
                canEdit = true;
            } else if (m_session.isDoctor()) {
                canEdit = visit.doctorId() == m_session.currentUser().id();
            }
        }
    }

    m_updateButton->setEnabled(canEdit);
    m_deleteButton->setEnabled(canEdit);
}

bool VisitsWidget::matchesVisitFilter(const Visit& visit, const QString& filter) const {
    const QString patientName = m_patientNames.value(visit.patientId(), QString("%1").arg(visit.patientId()));
    const QString doctorName = m_doctorNames.value(visit.doctorId(), QString("%1").arg(visit.doctorId()));
    const QString composite = QString("%1 %2 %3 %4 %5")
        .arg(patientName,
             doctorName,
             visit.visitDate().toString("yyyy-MM-dd HH:mm"),
             QString::number(visit.durationMinutes()),
             visit.notes());
    return composite.contains(filter, Qt::CaseInsensitive);
}
