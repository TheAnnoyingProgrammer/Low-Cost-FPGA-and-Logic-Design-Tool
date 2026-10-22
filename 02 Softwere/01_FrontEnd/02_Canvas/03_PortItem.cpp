#include "03_PortItem.h"
#include "00_Canvas.h"
#include "01_GateItem.h"
#include <QFont>
#include <QGraphicsScene>
#include <QPen>
#include <QtMath>
#include <algorithm>
#include <cmath>

PortItem::PortItem(const QString& name, Direction direction)
    : QGraphicsRectItem(0, 0, Board::PortZone, Board::PortHeight)
    , direction(direction)
{
    setPen(Qt::NoPen);
    setBrush(Qt::NoBrush);
    setFlag(ItemSendsGeometryChanges);
    setZValue(1);

    const bool isInput = direction == Direction::Input;

    auto* label = new QGraphicsSimpleTextItem(name, this);
    QFont font = label->font();
    font.setPixelSize(11);
    label->setFont(font);
    label->setBrush(QColor(38, 70, 30));
    const QRectF textBox = label->boundingRect();
    const double textY = (rect().height() - textBox.height()) / 2;
    label->setPos(isInput ? rect().width() - 10 - textBox.width() : 10, textY);

    auto* pin = new PinItem(this);
    pin->setToolTip(name);
    pin->setPos(isInput ? rect().width() : 0, rect().height() / 2);
}

double PortItem::edgeX() const
{
    return direction == Direction::Input ? 0.0 : Board::Width - Board::PortZone;
}

QVariant PortItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
    {
        const double half = Board::PortHeight / 2;
        double dotY = qRound((value.toPointF().y() + half) / Board::Grid) * Board::Grid;
        dotY = std::clamp(dotY, Board::Grid * 2, Board::Height - Board::Grid * 2);
        const QPointF target(edgeX(), dotY - half);

        if (scene())
            for (QGraphicsItem* item : scene()->items())
                if (auto* other = qgraphicsitem_cast<PortItem*>(item))
                    if (other != this && other->direction == direction
                        && std::abs(other->pos().y() - target.y()) < Board::PortHeight)
                        return pos();

        return target;
    }
    if (change == ItemPositionHasChanged)
        updateWires();

    return QGraphicsRectItem::itemChange(change, value);
}