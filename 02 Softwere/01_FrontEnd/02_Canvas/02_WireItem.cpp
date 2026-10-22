#include "02_WireItem.h"
#include "01_GateItem.h"
#include <QPainter>
#include <QPainterPathStroker>
#include <QPen>
#include <algorithm>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

namespace
{
const QColor wireColour(27, 127, 59);
const QColor selectedColour(47, 125, 246);
const double looseEndSize = 6.0;
}

void WireOwner::attachWire(WireItem* wire)
{
    if (std::find(wires.begin(), wires.end(), wire) == wires.end())
        wires.push_back(wire);
}

void WireOwner::detachWire(WireItem* wire)
{
    wires.erase(std::remove(wires.begin(), wires.end(), wire), wires.end());
}

void WireOwner::updateWires()
{
    for (WireItem* wire : wires)
        wire->updateEnds();
}

WireItem::WireItem(int wireIndex, PinItem* startPin, PinItem* endPin, const std::vector<QPointF>& bends,
                   const QPointF& looseEnd, Project& project)
    : wireIndex(wireIndex)
    , start(startPin)
    , end(endPin)
    , bends(bends)
    , looseEnd(looseEnd)
    , project(project)
{
    setPen(QPen(wireColour, 2.0, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    setFlag(ItemIsSelectable);
    setZValue(-1);
    updateEnds();
}

QPainterPath WireItem::route(const std::vector<QPointF>& points)
{
    QPainterPath path;
    if (points.empty()) return path;

    path.moveTo(points.front());
    for (size_t i = 1; i < points.size(); ++i)
    {
        path.lineTo(points[i].x(), points[i - 1].y());
        path.lineTo(points[i]);
    }
    return path;
}

QPointF WireItem::endPoint() const
{
    return end ? end->scenePos() : looseEnd;
}

void WireItem::extend(const std::vector<QPointF>& newBends, PinItem* newEndPin, const QPointF& newLooseEnd)
{
    bends.push_back(endPoint());
    bends.insert(bends.end(), newBends.begin(), newBends.end());
    end = newEndPin;
    looseEnd = newLooseEnd;
    updateEnds();
}

void WireItem::updateEnds()
{
    const QPointF a = start->scenePos();
    const QPointF b = endPoint();

    std::vector<QPointF> points{ a };
    points.insert(points.end(), bends.begin(), bends.end());
    points.push_back(b);
    setPath(route(points));

    project.wires[wireIndex] = { { a.x(), a.y() }, { b.x(), b.y() } };
}

QPainterPath WireItem::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(8);
    return stroker.createStroke(path());
}

QRectF WireItem::boundingRect() const
{
    return shape().boundingRect();
}

void WireItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*)
{
    QPen drawPen = pen();
    if (isSelected()) drawPen.setColor(selectedColour);
    painter->setPen(drawPen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPath(path());

    if (!end)
    {
        painter->setPen(QPen(drawPen.color(), 1.5));
        painter->drawRect(QRectF(looseEnd.x() - looseEndSize / 2, looseEnd.y() - looseEndSize / 2,
                                 looseEndSize, looseEndSize));
    }
}