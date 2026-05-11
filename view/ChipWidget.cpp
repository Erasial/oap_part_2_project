#include "ChipWidget.hpp"

#include <QHBoxLayout>
#include <QLabel>
#include <QToolButton>

ChipWidget::ChipWidget(int id, const QString& text, QWidget* parent)
    : QFrame(parent),
      m_id(id) {
    setObjectName("chipWidget");
    setFrameShape(QFrame::NoFrame);

    m_label = new QLabel(text, this);
    m_removeButton = new QToolButton(this);
    m_removeButton->setText("x");
    m_removeButton->setAutoRaise(true);

    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(6, 2, 6, 2);
    layout->setSpacing(4);
    layout->addWidget(m_label);
    layout->addWidget(m_removeButton);

    connect(m_removeButton, &QToolButton::clicked, this, [this]() {
        emit removeRequested(m_id);
    });
}

int ChipWidget::id() const {
    return m_id;
}
