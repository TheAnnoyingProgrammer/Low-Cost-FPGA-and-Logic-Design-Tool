#pragma once

#include <QGraphicsPathItem>
#include <vector>

class PinItem;
class Project;
class WireItem;

class WireOwner
{
public:
    virtual ~WireOwner() = default;

    void attachWire(WireItem* wire);
    void detachWire(WireItem* wire);
    const std::vector<WireItem*>& attachedWires() const { return wires; }

protected:
    void updateWires();

private:
    std::vector<WireItem*> wires;
};

class WireItem : public QGraphicsPathItem
{
public:
    enum { Type = UserType + 3 };

    WireItem(int wireIndex, PinItem* startPin, PinItem* endPin, const std::vector<QPointF>& bends,
             const QPointF& looseEnd, Project& project);
    int type() const override { return Type; }

    static QPainterPath route(const std::vector<QPointF>& points);

    void extend(const std::vector<QPointF>& newBends, PinItem* newEndPin, const QPointF& newLooseEnd);

    void updateEnds();

    int index() const { return wireIndex; }
    void setIndex(int newIndex) { wireIndex = newIndex; }
    PinItem* startPin() const { return start; }
    PinItem* endPin() const { return end; }
    QPointF endPoint() const;

    QRectF boundingRect() const override;
    QPainterPath shape() const override;
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

private:
    int wireIndex;
    PinItem* start;
    PinItem* end;
    std::vector<QPointF> bends;
    QPointF looseEnd;
    Project& project;
};