#pragma once

#include <QGraphicsRectItem>
#include <QString>
#include "02_WireItem.h"

class PortItem : public QGraphicsRectItem, public WireOwner
{
public:
    enum { Type = UserType + 4 };
    enum class Direction { Input, Output };

    PortItem(const QString& name, Direction direction);
    int type() const override { return Type; }

protected:
    QVariant itemChange(GraphicsItemChange change, const QVariant& value) override;

private:
    double edgeX() const;
    Direction direction;
};