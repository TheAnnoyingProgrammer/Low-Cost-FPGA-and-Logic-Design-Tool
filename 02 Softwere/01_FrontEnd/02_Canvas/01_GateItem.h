#pragma once

#include <QGraphicsEllipseItem>
#include <QGraphicsRectItem>
#include "02_WireItem.h"

class Definition;
class Project;

class PinItem : public QGraphicsEllipseItem
{
public:
    enum { Type = UserType + 2 };

    explicit PinItem(QGraphicsItem* parent);
    int type() const override { return Type; }
};

class GateItem : public QGraphicsRectItem, public WireOwner
{
public:
    enum { Type = UserType + 1 };

    GateItem(int instanceIndex, const Definition& definition, Project& project);
    int type() const override { return Type; }

    int index() const { return instanceIndex; }
    void setIndex(int newIndex) { instanceIndex = newIndex; }

    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    int instanceIndex;
    Project& project;
};