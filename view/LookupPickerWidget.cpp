#include "LookupPickerWidget.hpp"

#include "ChipWidget.hpp"
#include "FlowLayout.hpp"

#include <QCompleter>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QStringListModel>
#include <QVBoxLayout>

LookupPickerWidget::LookupPickerWidget(QWidget* parent)
    : QWidget(parent) {
    m_input = new QLineEdit(this);
    m_model = new QStringListModel(this);
    m_completer = new QCompleter(m_model, this);
    m_completer->setCaseSensitivity(Qt::CaseInsensitive);
    m_completer->setFilterMode(Qt::MatchContains);
    m_input->setCompleter(m_completer);

    m_chipContainer = new QWidget(this);
    m_chipLayout = new FlowLayout(m_chipContainer, 0, 6);
    m_chipContainer->setLayout(m_chipLayout);

    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(6);
    layout->addWidget(m_input);
    layout->addWidget(m_chipContainer);

    connect(m_input, &QLineEdit::returnPressed, this, &LookupPickerWidget::handleAddFromInput);
    connect(m_completer, QOverload<const QString&>::of(&QCompleter::activated), this, [this](const QString& text) {
        m_input->setText(text);
        handleAddFromInput();
    });
}

void LookupPickerWidget::setItems(const QVector<LookupCode>& items, const QString& placeholderText) {
    m_idToLabel.clear();
    m_labelToId.clear();

    QStringList labels;
    labels.reserve(items.size());
    for (const auto& item : items) {
        const QString label = QString("%1 - %2").arg(item.code, item.name);
        m_idToLabel.insert(item.id, label);
        m_labelToId.insert(label.toLower(), item.id);
        labels.append(label);
    }

    m_model->setStringList(labels);
    m_input->setPlaceholderText(placeholderText);
}

QVector<int> LookupPickerWidget::selectedIds() const {
    QVector<int> ids;
    ids.reserve(m_chipWidgets.size());
    for (auto it = m_chipWidgets.constBegin(); it != m_chipWidgets.constEnd(); ++it) {
        ids.append(it.key());
    }
    return ids;
}

void LookupPickerWidget::setSelectedIds(const QVector<int>& ids) {
    clear();
    for (int id : ids) {
        addChip(id);
    }
}

void LookupPickerWidget::clear() {
    for (auto it = m_chipWidgets.begin(); it != m_chipWidgets.end(); ++it) {
        m_chipLayout->removeWidget(it.value());
        delete it.value();
    }
    m_chipWidgets.clear();
}

void LookupPickerWidget::handleAddFromInput() {
    const QString text = m_input->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    const int id = idForLabel(text);
    if (id <= 0) {
        m_input->clear();
        return;
    }

    addChip(id);
    m_input->clear();
    emit selectionChanged();
}

void LookupPickerWidget::addChip(int id) {
    if (m_chipWidgets.contains(id)) {
        return;
    }

    const QString label = m_idToLabel.value(id);
    if (label.isEmpty()) {
        return;
    }

    auto* chip = new ChipWidget(id, label, m_chipContainer);
    connect(chip, &ChipWidget::removeRequested, this, &LookupPickerWidget::removeChip);
    m_chipLayout->addWidget(chip);
    m_chipWidgets.insert(id, chip);
}

void LookupPickerWidget::removeChip(int id) {
    QWidget* chip = m_chipWidgets.take(id);
    if (!chip) {
        return;
    }
    m_chipLayout->removeWidget(chip);
    delete chip;
    emit selectionChanged();
}

int LookupPickerWidget::idForLabel(const QString& label) const {
    const QString key = label.toLower();
    return m_labelToId.value(key, 0);
}
