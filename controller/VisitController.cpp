#include "VisitController.hpp"

VisitController::VisitController(VisitRepository& repository, SessionContext& session)
    : m_repository(repository),
      m_session(session) {}

bool VisitController::createVisit(const Visit& visit, int& newId) {
    if (!m_session.isAdmin() && !m_session.isDoctor()) {
        return false;
    }

    if (m_session.isDoctor() && visit.doctorId() != m_session.currentUser().id()) {
        return false;
    }

    return m_repository.create(visit, newId);
}

bool VisitController::updateVisit(const Visit& visit) {
    if (!canMutateVisit(visit)) {
        return false;
    }

    return m_repository.update(visit);
}

bool VisitController::removeVisit(int visitId) {
    Visit visit;
    if (!m_repository.findById(visitId, visit)) {
        return false;
    }

    if (!canMutateVisit(visit)) {
        return false;
    }

    return m_repository.remove(visitId);
}

bool VisitController::findVisit(int visitId, Visit& visit) const {
    if (!m_session.isAdmin() && !m_session.isDoctor()) {
        return false;
    }

    return m_repository.findById(visitId, visit);
}

QVector<Visit> VisitController::listVisits() const {
    if (m_session.isAdmin()) {
        return m_repository.listAll();
    }

    if (m_session.isDoctor()) {
        return m_repository.listAll();
    }

    return {};
}

QVector<Visit> VisitController::listOwnVisits() const {
    if (m_session.isAdmin()) {
        return m_repository.listAll();
    }

    if (m_session.isDoctor()) {
        return m_repository.listByDoctor(m_session.currentUser().id());
    }

    return {};
}

QVector<Visit> VisitController::listByPatient(int patientId) const {
    if (!m_session.isAdmin() && !m_session.isDoctor()) {
        return {};
    }

    return m_repository.listByPatient(patientId);
}

bool VisitController::canMutateVisit(const Visit& visit) const {
    if (m_session.isAdmin()) {
        return true;
    }

    if (!m_session.isDoctor()) {
        return false;
    }

    return visit.doctorId() == m_session.currentUser().id();
}
