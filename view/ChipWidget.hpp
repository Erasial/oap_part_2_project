#pragma once

#include <QFrame>

class QLabel;
class QToolButton;

class ChipWidget : public QFrame {
    Q_OBJECT

public:
    ChipWidget(int id, const QString& text, QWidget* parent = nullptr);

    int id() const;

signals:
    void removeRequested(int id);

private:
    int m_id = 0;
    QLabel* m_label = nullptr;
    QToolButton* m_removeButton = nullptr;
};
