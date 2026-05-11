#pragma once

#include <QWidget>
#include <QHash>
#include <QVector>

#include "controller/LookupRepository.hpp"

class QCompleter;
class QLineEdit;
class QStringListModel;

class FlowLayout;

class LookupPickerWidget : public QWidget {
    Q_OBJECT

public:
    explicit LookupPickerWidget(QWidget* parent = nullptr);

    void setItems(const QVector<LookupCode>& items, const QString& placeholderText);
    QVector<int> selectedIds() const;
    void setSelectedIds(const QVector<int>& ids);
    void clear();

signals:
    void selectionChanged();

private slots:
    void handleAddFromInput();

private:
    void addChip(int id);
    void removeChip(int id);
    int idForLabel(const QString& label) const;

    QLineEdit* m_input = nullptr;
    QStringListModel* m_model = nullptr;
    QCompleter* m_completer = nullptr;
    QWidget* m_chipContainer = nullptr;
    FlowLayout* m_chipLayout = nullptr;

    QHash<int, QString> m_idToLabel;
    QHash<QString, int> m_labelToId;
    QHash<int, QWidget*> m_chipWidgets;
};
