#pragma once

#include <QGraphicsPathItem>

class PinItem;
class Project;

// Wire between two pins, drawn KiCad style: horizontal then vertical (L shape).
// Keeps its Wire (start/end points) in the project up to date.
class WireItem : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 3 };

    WireItem(int wireIndex, PinItem* startPin, PinItem* endPin, Project& project);
    int type() const override { return Type; }

    static QPainterPath route(const QPointF& start, const QPointF& end);

    void updateEnds();
    QPainterPath shape() const override;   // clickable along the line only

    int index() const { return wireIndex; }
    void setIndex(int newIndex) { wireIndex = newIndex; }
    PinItem* startPin() const { return start; }
    PinItem* endPin() const { return end; }

private:
    int wireIndex;   // index into project.wires
    PinItem* start;
    PinItem* end;
    Project& project;
};