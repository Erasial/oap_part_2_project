#pragma once

#include <QLayout>
#include <QList>

class FlowLayout : public QLayout {
    Q_OBJECT

public:
    explicit FlowLayout(QWidget* parent = nullptr, int margin = 0, int spacing = -1);
    ~FlowLayout() override;

    void addItem(QLayoutItem* item) override;
    int count() const override;
    QLayoutItem* itemAt(int index) const override;
    QLayoutItem* takeAt(int index) override;
    Qt::Orientations expandingDirections() const override;
    QSize sizeHint() const override;
    QSize minimumSize() const override;
    void setGeometry(const QRect& rect) override;

private:
    QSize doLayout(const QRect& rect, bool testOnly) const;
    int horizontalSpacing() const;
    int verticalSpacing() const;

    QList<QLayoutItem*> m_items;
};
