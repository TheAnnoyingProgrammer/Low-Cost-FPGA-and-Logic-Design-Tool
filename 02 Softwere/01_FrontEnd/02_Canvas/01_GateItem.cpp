#include "01_GateItem.h"
#include "00_Canvas.h"
#include <QBrush>
#include <QPainter>
#include <QPen>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

namespace
{
const double pinRadius = 4.5;
const double pinSpacing = 20.0;
const QColor fillColour(238, 243, 255);
const QColor edgeColour(70, 85, 110);
const QColor selectedColour(47, 125, 246);
const QColor textColour(35, 50, 77);
}

PinItem::PinItem(QGraphicsItem* parent)
    : QGraphicsEllipseItem(-pinRadius, -pinRadius, 2 * pinRadius, 2 * pinRadius, parent)
{
    setBrush(QBrush(QColor(43, 47, 54)));
    setPen(Qt::NoPen);
}

GateItem::GateItem(int instanceIndex, const Definition& definition, Project& project)
    : QGraphicsRectItem(0, 0, definition.symbol.width, definition.symbol.height)
    , instanceIndex(instanceIndex)
    , project(project)
{
    setPen(QPen(edgeColour, 2.5));
    setFlags(ItemIsSelectable | ItemSendsGeometryChanges);

    auto* label = new QGraphicsSimpleTextItem(QString::fromStdString(definition.name), this);
    label->setBrush(textColour);
    const QRectF textBox = label->boundingRect();
    label->setPos((rect().width() - textBox.width()) / 2, (rect().height() - textBox.height()) / 2);

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

void GateItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    painter->setRenderHint(QPainter::Antialiasing);
    painter->setBrush(fillColour);
    painter->setPen(isSelected() ? QPen(selectedColour, 2.5) : QPen(edgeColour, 1.5));
    painter->drawRoundedRect(rect(), 4, 4);
}

QVariant GateItem::itemChange(GraphicsItemChange change, const QVariant& value)
{
    if (change == ItemPositionChange)
        return Canvas::constrainGatePosition(value.toPointF(), rect().size());

    if (change == ItemPositionHasChanged)
    {
        project.instances[instanceIndex].position = { pos().x(), pos().y() };
        updateWires();
    }
    return QGraphicsRectItem::itemChange(change, value);
}