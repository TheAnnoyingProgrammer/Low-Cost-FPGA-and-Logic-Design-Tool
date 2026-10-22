#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include <vector>

class Definition;
class Project;
class WireItem;

// Small dot on a gate or port. Press on it and drag to another pin to make a wire.
class PinItem : public QGraphicsEllipseItem
{
public:
    enum { Type = UserType + 2 };

    explicit PinItem(QGraphicsItem* parent);
    int type() const override { return Type; }
};

// A gate drawn as a rectangle with a label and pin dots. Draggable, snaps to grid.
class GateItem : public QGraphicsRectItem
{
public:
    enum { Type = UserType + 1 };

    GateItem(int instanceIndex, const Definition& definition, Project& project);
    int type() const override { return Type; }

    int index() const { return instanceIndex; }
    void setIndex(int newIndex) { instanceIndex = newIndex; }

    void attachWire(WireItem* wire);
    void detachWire(WireItem* wire);
    const std::vector<WireItem*>& attachedWires() const { return wires; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int instanceIndex;            // index into project.instances
    Project& project;
    std::vector<WireItem*> wires; // wires to redraw when this gate moves
};