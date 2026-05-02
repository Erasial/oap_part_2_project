#include "Visit.hpp"

Visit::Visit(int id, int patientId, int doctorId, const QDateTime& visitDate, int durationMinutes, const QString& notes)
	: m_id(id),
	  m_patientId(patientId),
	  m_doctorId(doctorId),
	  m_visitDate(visitDate),
	  m_durationMinutes(durationMinutes),
	  m_notes(notes) {}

int Visit::id() const {
	return m_id;
}

void Visit::setId(int id) {
	m_id = id;
}

int Visit::patientId() const {
	return m_patientId;
}

void Visit::setPatientId(int patientId) {
	m_patientId = patientId;
}

int Visit::doctorId() const {
	return m_doctorId;
}

void Visit::setDoctorId(int doctorId) {
	m_doctorId = doctorId;
}

const QDateTime& Visit::visitDate() const {
	return m_visitDate;
}

void Visit::setVisitDate(const QDateTime& visitDate) {
	m_visitDate = visitDate;
}

int Visit::durationMinutes() const {
	return m_durationMinutes;
}

void Visit::setDurationMinutes(int durationMinutes) {
	m_durationMinutes = durationMinutes;
}

const QString& Visit::notes() const {
	return m_notes;
}

void Visit::setNotes(const QString& notes) {
	m_notes = notes;
}

const QVector<int>& Visit::diagnosisIds() const {
	return m_diagnosisIds;
}

void Visit::setDiagnosisIds(const QVector<int>& diagnosisIds) {
	m_diagnosisIds = diagnosisIds;
}

const QVector<int>& Visit::reasonIds() const {
	return m_reasonIds;
}

void Visit::setReasonIds(const QVector<int>& reasonIds) {
	m_reasonIds = reasonIds;
}

const QVector<int>& Visit::procedureIds() const {
	return m_procedureIds;
}

void Visit::setProcedureIds(const QVector<int>& procedureIds) {
	m_procedureIds = procedureIds;
}
