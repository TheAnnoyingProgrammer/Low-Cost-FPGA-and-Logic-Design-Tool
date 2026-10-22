#include "01_GateItem.h"
#include "00_Canvas.h"
#include "02_WireItem.h"
#include <QBrush>
#include <QCursor>
#include <QPen>
#include <algorithm>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

namespace
{
const double pinRadius = 5.0;
const double pinSpacing = 20.0;
}

PinItem::PinItem(QGraphicsItem* parent)
    : QGraphicsEllipseItem(-pinRadius, -pinRadius, 2 * pinRadius, 2 * pinRadius, parent)
{
    setBrush(QBrush(QColor(40, 40, 40)));
    setPen(Qt::NoPen);
    setCursor(Qt::CrossCursor);
}

GateItem::GateItem(int instanceIndex, const Definition& definition, Project& project)
    : QGraphicsRectItem(0, 0, definition.symbol.width, definition.symbol.height)
    , instanceIndex(instanceIndex)
    , project(project)
{
    setBrush(QBrush(QColor(235, 242, 255)));
    setPen(QPen(Qt::black, 1.5));
    setFlags(ItemIsMovable | ItemSendsGeometryChanges);

    auto* label = new QGraphicsSimpleTextItem(QString::fromStdString(definition.name), this);
    const QRectF textBox = label->boundingRect();
    label->setPos((rect().width() - textBox.width()) / 2, (rect().height() - textBox.height()) / 2);

    // Pin positions are worked out here: inputs down the left edge, output centred on the right.
    int inputIndex = 0;
    for (const Pin& pin : definition.pins)
    {
        auto* dot = new PinItem(this);
        dot->setToolTip(QString::fromStdString(pin.name));
        if (pin.type == PinType::Input)
            dot->setPos(0, pinSpacing / 2 + pinSpacing * inputIndex++);
        else
            dot->setPos(rect().width(), rect().height() / 2);
    }
}

void GateItem::attachWire(WireItem* wire)
{
    wires.push_back(wire);
}

void GateItem::detachWire(WireItem* wire)
{
    wires.erase(std::remove(wires.begin(), wires.end(), wire), wires.end());
}

QVariant GateItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
        return Canvas::constrainGatePosition(value.toPointF(), rect().size());   // snap + stay on board

    if (change == ItemPositionHasChanged)
    {
        project.instances[instanceIndex].position = { pos().x(), pos().y() };
        for (WireItem* wire : wires)
            wire->updateEnds();
    }
    return QGraphicsRectItem::itemChange(change, value);
}