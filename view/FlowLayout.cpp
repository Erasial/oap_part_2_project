#include "FlowLayout.hpp"

#include <QStyle>
#include <QWidget>

FlowLayout::FlowLayout(QWidget* parent, int margin, int spacing)
    : QLayout(parent) {
    setContentsMargins(margin, margin, margin, margin);
    setSpacing(spacing);
}

FlowLayout::~FlowLayout() {
    while (!m_items.isEmpty()) {
        delete m_items.takeFirst();
    }
}

void FlowLayout::addItem(QLayoutItem* item) {
    m_items.append(item);
}

int FlowLayout::count() const {
    return m_items.count();
}

QLayoutItem* FlowLayout::itemAt(int index) const {
    return m_items.value(index);
}

QLayoutItem* FlowLayout::takeAt(int index) {
    if (index >= 0 && index < m_items.size()) {
        return m_items.takeAt(index);
    }
    return nullptr;
}

Qt::Orientations FlowLayout::expandingDirections() const {
    return Qt::Horizontal | Qt::Vertical;
}

QSize FlowLayout::sizeHint() const {
    const int width = contentsRect().width() > 0 ? contentsRect().width() : 300;
    return doLayout(QRect(0, 0, width, 0), true);
}

QSize FlowLayout::minimumSize() const {
    QSize size;
    for (QLayoutItem* item : m_items) {
        size = size.expandedTo(item->minimumSize());
    }
    const QMargins margins = contentsMargins();
    size += QSize(margins.left() + margins.right(), margins.top() + margins.bottom());
    return size;
}

void FlowLayout::setGeometry(const QRect& rect) {
    QLayout::setGeometry(rect);
    doLayout(rect, false);
}

QSize FlowLayout::doLayout(const QRect& rect, bool testOnly) const {
    int left = 0;
    int top = 0;
    int right = 0;
    int bottom = 0;
    getContentsMargins(&left, &top, &right, &bottom);

    const QRect effectiveRect = rect.adjusted(left, top, -right, -bottom);
    int x = effectiveRect.x();
    int y = effectiveRect.y();
    int lineHeight = 0;

    for (QLayoutItem* item : m_items) {
        const int spaceX = horizontalSpacing();
        const int spaceY = verticalSpacing();
        const int nextX = x + item->sizeHint().width() + spaceX;

        if (nextX - spaceX > effectiveRect.right() && lineHeight > 0) {
            x = effectiveRect.x();
            y += lineHeight + spaceY;
            lineHeight = 0;
        }

        if (!testOnly) {
            item->setGeometry(QRect(x, y, item->sizeHint().width(), item->sizeHint().height()));
        }

        x += item->sizeHint().width() + spaceX;
        lineHeight = qMax(lineHeight, item->sizeHint().height());
    }

    return QSize(x, y + lineHeight);
}

int FlowLayout::horizontalSpacing() const {
    if (spacing() >= 0) {
        return spacing();
    }
    const QWidget* parent = parentWidget();
    if (parent) {
        return parent->style()->pixelMetric(QStyle::PM_LayoutHorizontalSpacing, nullptr, parent);
    }
    return QStyle::PM_LayoutHorizontalSpacing;
}

int FlowLayout::verticalSpacing() const {
    if (spacing() >= 0) {
        return spacing();
    }
    const QWidget* parent = parentWidget();
    if (parent) {
        return parent->style()->pixelMetric(QStyle::PM_LayoutVerticalSpacing, nullptr, parent);
    }
    return QStyle::PM_LayoutVerticalSpacing;
}
