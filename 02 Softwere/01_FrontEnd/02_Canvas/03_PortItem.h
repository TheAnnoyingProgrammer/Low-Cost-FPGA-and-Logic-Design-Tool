#pragma once

#include <QGraphicsRectItem>
#include <QString>

// Fixed FPGA I/O port on the board edge: a labelled box with one pin dot.
// Inputs sit on the left edge (pin on their right side), outputs on the right edge.
class PortItem : public QGraphicsRectItem
{
public:
    enum { Type = UserType + 4 };
    enum class Direction { Input, Output };

    PortItem(const QString& name, Direction direction);
    int type() const override { return Type; }
};
