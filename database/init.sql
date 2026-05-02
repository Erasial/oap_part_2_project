CREATE TABLE roles (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	role_name TEXT NOT NULL UNIQUE
);

CREATE TABLE users (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	username TEXT NOT NULL UNIQUE,
	password_hash TEXT NOT NULL,
	full_name TEXT NOT NULL,
	role_id INTEGER,
	FOREIGN KEY (role_id) REFERENCES roles (id)
);

CREATE TABLE doctor_profiles (
	user_id INTEGER PRIMARY KEY,
	specialization TEXT NOT NULL,
	FOREIGN KEY (user_id) REFERENCES users (id) ON DELETE CASCADE
);

CREATE TABLE patients (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	first_name TEXT NOT NULL,
	last_name TEXT NOT NULL,
	birthdate DATE NOT NULL,
	phone TEXT
);

CREATE TABLE visits (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	patient_id INTEGER,
	doctor_id INTEGER,
	visit_date DATETIME NOT NULL,
	duration_minutes INTEGER NOT NULL,
	notes TEXT,
	FOREIGN KEY (patient_id) REFERENCES patients (id),
	FOREIGN KEY (doctor_id) REFERENCES users (id)
);

CREATE TABLE icd10_codes (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	code TEXT NOT NULL UNIQUE,
	name TEXT NOT NULL
);

CREATE TABLE icpc2_codes (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	code TEXT NOT NULL UNIQUE,
	name TEXT NOT NULL
);

CREATE TABLE achi_codes (
	id INTEGER PRIMARY KEY AUTOINCREMENT,
	code TEXT NOT NULL UNIQUE,
	name TEXT NOT NULL
);

CREATE TABLE visit_diagnoses (
	visit_id INTEGER NOT NULL,
	icd10_id INTEGER NOT NULL,
	PRIMARY KEY (visit_id, icd10_id),
	FOREIGN KEY (visit_id) REFERENCES visits (id) ON DELETE CASCADE,
	FOREIGN KEY (icd10_id) REFERENCES icd10_codes (id)
);

CREATE TABLE visit_reasons (
	visit_id INTEGER NOT NULL,
	icpc2_id INTEGER NOT NULL,
	PRIMARY KEY (visit_id, icpc2_id),
	FOREIGN KEY (visit_id) REFERENCES visits (id) ON DELETE CASCADE,
	FOREIGN KEY (icpc2_id) REFERENCES icpc2_codes (id)
);

CREATE TABLE visit_procedures (
	visit_id INTEGER NOT NULL,
	achi_id INTEGER NOT NULL,
	PRIMARY KEY (visit_id, achi_id),
	FOREIGN KEY (visit_id) REFERENCES visits (id) ON DELETE CASCADE,
	FOREIGN KEY (achi_id) REFERENCES achi_codes (id)
);

CREATE INDEX idx_users_role_id ON users (role_id);
CREATE INDEX idx_visits_patient_id ON visits (patient_id);
CREATE INDEX idx_visits_doctor_id ON visits (doctor_id);
CREATE INDEX idx_visits_visit_date ON visits (visit_date);
CREATE INDEX idx_visit_diagnoses_icd10_id ON visit_diagnoses (icd10_id);
CREATE INDEX idx_visit_reasons_icpc2_id ON visit_reasons (icpc2_id);
CREATE INDEX idx_visit_procedures_achi_id ON visit_procedures (achi_id);

INSERT INTO roles (role_name) VALUES ('admin');
INSERT INTO roles (role_name) VALUES ('doctor');

INSERT INTO users (username, password_hash, full_name, role_id)
VALUES (
	'admin',
	'gcrypt$120000$56659c06d906c404f332c7e0218b1392$dd515a2a7f6afadb6890f6ffb81dbed4fcb00a3c757b3ce3e79dbfae85fd9a86',
	'System Administrator',
	(SELECT id FROM roles WHERE role_name = 'admin')
);
	