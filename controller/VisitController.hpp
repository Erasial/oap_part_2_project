#pragma once

#include "controller/VisitRepository.hpp"
#include "controller/SessionContext.hpp"

#include <QVector>

class VisitController {
public:
    VisitController(VisitRepository& repository, SessionContext& session);

    bool createVisit(const Visit& visit, int& newId);
    bool updateVisit(const Visit& visit);
    bool removeVisit(int visitId);

    bool findVisit(int visitId, Visit& visit) const;
    QVector<Visit> listVisits() const;
    QVector<Visit> listOwnVisits() const;

private:
    bool canMutateVisit(const Visit& visit) const;

    VisitRepository& m_repository;
    SessionContext& m_session;
};
