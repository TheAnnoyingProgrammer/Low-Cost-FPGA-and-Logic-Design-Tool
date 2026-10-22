#include "02_WireItem.h"
#include "01_GateItem.h"
#include <QPainterPathStroker>
#include <QPen>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

WireItem::WireItem(int wireIndex, PinItem* startPin, PinItem* endPin, Project& project)
    : wireIndex(wireIndex)
    , start(startPin)
    , end(endPin)
    , project(project)
{
    setPen(QPen(QColor(20, 120, 40), 2.5, Qt::SolidLine, Qt::SquareCap, Qt::MiterJoin));
    setZValue(-1);   // under gates so the pin dots stay visible
    updateEnds();
}

QPainterPath WireItem::route(const QPointF& start, const QPointF& end)
{
    QPainterPath path(start);
    path.lineTo(end.x(), start.y());   // horizontal first
    path.lineTo(end);                  // then vertical
    return path;
}

void WireItem::updateEnds()
{
    const QPointF a = start->scenePos();
    const QPointF b = end->scenePos();
    setPath(route(a, b));
    project.wires[wireIndex] = { { a.x(), a.y() }, { b.x(), b.y() } };
}

QPainterPath WireItem::shape() const
{
    QPainterPathStroker stroker;
    stroker.setWidth(8);
    return stroker.createStroke(path());
}