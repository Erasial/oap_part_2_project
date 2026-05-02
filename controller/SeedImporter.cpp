#include "SeedImporter.hpp"

#include <QFile>
#include <QTextStream>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>

namespace {
QStringList parseCsvLine(const QString& line) {
    QStringList fields;
    QString current;
    bool inQuotes = false;

    for (int i = 0; i < line.size(); ++i) {
        const QChar ch = line.at(i);
        if (ch == '"') {
            if (inQuotes && i + 1 < line.size() && line.at(i + 1) == '"') {
                current.append('"');
                ++i;
            } else {
                inQuotes = !inQuotes;
            }
        } else if (ch == ',' && !inQuotes) {
            fields.append(current);
            current.clear();
        } else {
            current.append(ch);
        }
    }

    fields.append(current);
    return fields;
}
}

SeedImporter::SeedImporter(QSqlDatabase db)
    : m_db(db) {}

bool SeedImporter::importIfEmpty(const QString& tableName, const QString& csvPath) {
    if (!m_db.isOpen()) {
        return false;
    }

    QSqlQuery countQuery(m_db);
    countQuery.prepare("SELECT COUNT(*) FROM " + tableName);
    if (!countQuery.exec() || !countQuery.next()) {
        return false;
    }

    if (countQuery.value(0).toInt() > 0) {
        return true;
    }

    return importTwoColumnCsv(tableName, csvPath);
}

bool SeedImporter::importTwoColumnCsv(const QString& tableName, const QString& csvPath) {
    QFile file(csvPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QTextStream stream(&file);
    const QString headerLine = stream.readLine();
    if (headerLine.isEmpty()) {
        return false;
    }

    QSqlQuery insertQuery(m_db);
    const QString statement = "INSERT OR IGNORE INTO " + tableName + " (code, name) VALUES (?, ?)";
    if (!insertQuery.prepare(statement)) {
        return false;
    }

    if (!m_db.transaction()) {
        return false;
    }

    while (!stream.atEnd()) {
        const QString line = stream.readLine();
        if (line.trimmed().isEmpty()) {
            continue;
        }

        const QStringList fields = parseCsvLine(line);
        if (fields.size() < 2) {
            continue;
        }

        const QString code = fields.at(0).trimmed();
        const QString name = fields.at(1).trimmed();
        if (code.isEmpty() || name.isEmpty()) {
            continue;
        }

        insertQuery.addBindValue(code);
        insertQuery.addBindValue(name);
        if (!insertQuery.exec()) {
            m_db.rollback();
            return false;
        }
        insertQuery.clear();
    }

    if (!m_db.commit()) {
        return false;
    }

    return true;
}
