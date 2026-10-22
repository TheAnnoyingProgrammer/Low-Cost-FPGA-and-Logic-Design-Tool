#include "03_PortItem.h"
#include "00_Canvas.h"
#include "01_GateItem.h"
#include <QBrush>
#include <QPen>

PortItem::PortItem(const QString& name, Direction direction)
    : QGraphicsRectItem(0, 0, Board::PortWidth, Board::PortHeight)
{
    const bool isInput = direction == Direction::Input;
    setBrush(QBrush(isInput ? QColor(255, 240, 220) : QColor(225, 245, 225)));
    setPen(QPen(Qt::black, 1));

    auto* label = new QGraphicsSimpleTextItem(name, this);
    const QRectF textBox = label->boundingRect();
    label->setPos((rect().width() - textBox.width()) / 2, (rect().height() - textBox.height()) / 2);

    auto* pin = new PinItem(this);
    pin->setToolTip(name);
    pin->setPos(isInput ? rect().width() : 0, rect().height() / 2);
}
