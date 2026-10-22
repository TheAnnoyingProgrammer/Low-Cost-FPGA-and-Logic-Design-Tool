#include "00_Canvas.h"
#include "01_GateItem.h"
#include "02_WireItem.h"
#include "03_PortItem.h"
#include <QAction>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <algorithm>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

Canvas::Canvas(Project& project, QWidget* parent)
    : QGraphicsView(parent)
    , project(project)
    , graphicsScene(new QGraphicsScene(this))
{
    graphicsScene->setSceneRect(0, 0, Board::Width, Board::Height);
    setScene(graphicsScene);
    setRenderHint(QPainter::Antialiasing);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    addPorts();
}

QPointF Canvas::constrainGatePosition(const QPointF& position, const QSizeF& size)
{
    const double x = qRound(position.x() / Board::Grid) * Board::Grid;
    const double y = qRound(position.y() / Board::Grid) * Board::Grid;
    return QPointF(std::clamp(x, Board::PortZone, Board::Width - Board::PortZone - size.width()),
                   std::clamp(y, 0.0, Board::Height - size.height()));
}

void Canvas::addPorts()
{
    for (int i = 0; i < Board::PortCount; ++i)
    {
        const double y = Board::FirstPortY + Board::PortSpacing * i - Board::PortHeight / 2;

        auto* input = new PortItem("IN" + QString::number(i), PortItem::Direction::Input);
        input->setPos(0, y);
        graphicsScene->addItem(input);

        auto* output = new PortItem("OUT" + QString::number(i), PortItem::Direction::Output);
        output->setPos(Board::Width - Board::PortWidth, y);
        graphicsScene->addItem(output);
    }
}

void Canvas::addGate(const std::string& definitionId)
{
    const Definition* definition = project.findDefinition(definitionId);
    if (!definition) return;

    // Drop near the middle of the board, stepping a little so gates don't stack exactly.
    const double step = 20.0 * (placedCount++ % 5);
    const QSizeF size(definition->symbol.width, definition->symbol.height);
    const QPointF position = constrainGatePosition(QPointF(Board::Width / 2 + step, Board::Height / 2 + step), size);

    Instance* instance = project.place(definitionId, { position.x(), position.y() });
    if (!instance || !instance->definition) return;

    auto* gate = new GateItem(static_cast<int>(gates.size()), *instance->definition, project);
    gate->setPos(position);
    graphicsScene->addItem(gate);
    gates.push_back(gate);
}

void Canvas::setDeleteAction(QAction* action)
{
    deleteAction = action;
}

void Canvas::setDeleteMode(bool on)
{
    deleteMode = on;
    cancelWire();
    viewport()->setCursor(on ? Qt::ForbiddenCursor : Qt::ArrowCursor);
}

PinItem* Canvas::pinAt(const QPoint& viewPosition) const
{
    for (QGraphicsItem* item : items(viewPosition))
        if (auto* pin = qgraphicsitem_cast<PinItem*>(item)) return pin;
    return nullptr;
}

void Canvas::connectPins(PinItem* startPin, PinItem* endPin)
{
    project.addWire({ 0, 0 }, { 0, 0 });   // real ends are filled in by WireItem
    auto* wire = new WireItem(static_cast<int>(wires.size()), startPin, endPin, project);
    graphicsScene->addItem(wire);
    wires.push_back(wire);

    // Ports never move, so only gates need to know about their wires.
    if (auto* gate = qgraphicsitem_cast<GateItem*>(startPin->parentItem()))
        gate->attachWire(wire);
    if (endPin->parentItem() != startPin->parentItem())
        if (auto* gate = qgraphicsitem_cast<GateItem*>(endPin->parentItem()))
            gate->attachWire(wire);
}

void Canvas::cancelWire()
{
    delete previewPath;
    previewPath = nullptr;
    wireStart = nullptr;
}

void Canvas::deleteAt(const QPoint& viewPosition)
{
    for (QGraphicsItem* item : items(viewPosition))   // topmost first
    {
        if (auto* wire = qgraphicsitem_cast<WireItem*>(item))
        {
            deleteWire(wire);
            return;
        }

        QGraphicsItem* top = item;                    // a click on a pin or label means its gate
        while (top->parentItem()) top = top->parentItem();
        if (auto* gate = qgraphicsitem_cast<GateItem*>(top))
        {
            deleteGate(gate);
            return;
        }
        if (qgraphicsitem_cast<PortItem*>(top)) return;   // ports can't be deleted
    }
}

void Canvas::deleteWire(WireItem* wire)
{
    for (PinItem* pin : { wire->startPin(), wire->endPin() })
        if (auto* gate = qgraphicsitem_cast<GateItem*>(pin->parentItem()))
            gate->detachWire(wire);

    const int index = wire->index();
    project.wires.erase(project.wires.begin() + index);
    wires.erase(wires.begin() + index);
    for (int i = index; i < static_cast<int>(wires.size()); ++i)
        wires[i]->setIndex(i);

    delete wire;
}

void Canvas::deleteGate(GateItem* gate)
{
    const std::vector<WireItem*> attached = gate->attachedWires();   // copy: deleteWire edits the list
    for (WireItem* wire : attached)
        deleteWire(wire);

    const int index = gate->index();
    project.instances.erase(project.instances.begin() + index);
    gates.erase(gates.begin() + index);
    for (int i = index; i < static_cast<int>(gates.size()); ++i)
        gates[i]->setIndex(i);

    delete gate;
}

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        if (deleteMode)
        {
            deleteAt(event->pos());
            return;
        }
        if (PinItem* pin = pinAt(event->pos()))
        {
            wireStart = pin;
            previewPath = graphicsScene->addPath(QPainterPath(), QPen(Qt::gray, 2, Qt::DashLine));
            return;   // don't let the gate start dragging
        }
    }
    QGraphicsView::mousePressEvent(event);
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    if (previewPath)
    {
        previewPath->setPath(WireItem::route(wireStart->scenePos(), mapToScene(event->pos())));
        return;
    }
    QGraphicsView::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (previewPath)
    {
        PinItem* end = pinAt(event->pos());
        if (end && end != wireStart)
            connectPins(wireStart, end);
        cancelWire();
        return;
    }
    QGraphicsView::mouseReleaseEvent(event);
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
    {
        cancelWire();
        if (deleteAction) deleteAction->setChecked(false);   // also calls setDeleteMode(false)
        else setDeleteMode(false);
        return;
    }
    QGraphicsView::keyPressEvent(event);
}

void Canvas::resizeEvent(QResizeEvent* event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(sceneRect(), Qt::KeepAspectRatio);   // always show the whole board
}

void Canvas::drawBackground(QPainter* painter, const QRectF& rect)
{
    const QRectF board(0, 0, Board::Width, Board::Height);
    painter->fillRect(rect, QColor(200, 200, 200));   // outside the board
    painter->fillRect(board, Qt::white);

    // Grid dots
    painter->setPen(QPen(QColor(190, 190, 190), 1));
    const QRectF area = rect.intersected(board);
    const double left = std::floor(area.left() / Board::Grid) * Board::Grid;
    const double top = std::floor(area.top() / Board::Grid) * Board::Grid;
    QVector<QPointF> dots;
    for (double x = left; x <= area.right(); x += Board::Grid)
        for (double y = top; y <= area.bottom(); y += Board::Grid)
            dots.append(QPointF(x, y));
    painter->drawPoints(dots.data(), static_cast<int>(dots.size()));

    painter->setPen(QPen(Qt::black, 2));
    painter->drawRect(board);
}