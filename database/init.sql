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
	notes TEXT,
	FOREIGN KEY (patient_id) REFERENCES patients (id),
	FOREIGN KEY (doctor_id) REFERENCES users (id)
);
	