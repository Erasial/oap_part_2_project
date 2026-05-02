#pragma once

#include <QDateTime>
#include <QVector>
#include <QString>

class Visit {
public:
	Visit() = default;
	Visit(int id, int patientId, int doctorId, const QDateTime& visitDate, int durationMinutes, const QString& notes);

	int id() const;
	void setId(int id);

	int patientId() const;
	void setPatientId(int patientId);

	int doctorId() const;
	void setDoctorId(int doctorId);

	const QDateTime& visitDate() const;
	void setVisitDate(const QDateTime& visitDate);

	int durationMinutes() const;
	void setDurationMinutes(int durationMinutes);

	const QString& notes() const;
	void setNotes(const QString& notes);

	const QVector<int>& diagnosisIds() const;
	void setDiagnosisIds(const QVector<int>& diagnosisIds);

	const QVector<int>& reasonIds() const;
	void setReasonIds(const QVector<int>& reasonIds);

	const QVector<int>& procedureIds() const;
	void setProcedureIds(const QVector<int>& procedureIds);

private:
	int m_id = 0;
	int m_patientId = 0;
	int m_doctorId = 0;
	QDateTime m_visitDate;
	int m_durationMinutes = 0;
	QString m_notes;
	QVector<int> m_diagnosisIds;
	QVector<int> m_reasonIds;
	QVector<int> m_procedureIds;
};
