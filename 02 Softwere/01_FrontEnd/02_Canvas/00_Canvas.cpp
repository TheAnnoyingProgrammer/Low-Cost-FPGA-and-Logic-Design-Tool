#include "00_Canvas.h"
#include "01_GateItem.h"
#include "02_WireItem.h"
#include "03_PortItem.h"
#include <QCursor>
#include <QGraphicsPathItem>
#include <QGraphicsScene>
#include <QKeyEvent>
#include <QMouseEvent>
#include <QPainter>
#include <QPen>
#include <QtMath>
#include <algorithm>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

namespace
{
const QColor outsideColour(233, 235, 239);
const QColor boardColour(250, 250, 251);
const QColor gridColour(208, 212, 219);
const QColor stripColour(178, 216, 160);

void attach(PinItem* pin, WireItem* wire)
{
    if (pin)
        if (auto* owner = dynamic_cast<WireOwner*>(pin->parentItem()))
            owner->attachWire(wire);
}

void detach(PinItem* pin, WireItem* wire)
{
    if (pin)
        if (auto* owner = dynamic_cast<WireOwner*>(pin->parentItem()))
            owner->detachWire(wire);
}
}

Canvas::Canvas(Project& project, QWidget* parent)
    : QGraphicsView(parent)
    , project(project)
    , graphicsScene(new QGraphicsScene(this))
{
    graphicsScene->setSceneRect(0, 0, Board::Width, Board::Height);
    setScene(graphicsScene);
    setRenderHint(QPainter::Antialiasing);
    setFrameShape(QFrame::NoFrame);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    viewport()->setMouseTracking(true);   // things follow the mouse without a button held
    addPorts();
    updateCursor();
}

// ---------------------------------------------------------------- helpers

QPointF Canvas::snapToGrid(const QPointF& point)
{
    return QPointF(qRound(point.x() / Board::Grid) * Board::Grid,
                   qRound(point.y() / Board::Grid) * Board::Grid);
}

QPointF Canvas::constrainGatePosition(const QPointF& position, const QSizeF& size)
{
    const QPointF snapped = snapToGrid(position);
    const double minX = Board::PortZone + Board::GateMargin;
    const double maxX = Board::Width - Board::PortZone - Board::GateMargin - size.width();
    return QPointF(std::clamp(snapped.x(), minX, maxX),
                   std::clamp(snapped.y(), Board::Grid, Board::Height - Board::Grid - size.height()));
}

QPointF Canvas::constrainWirePoint(const QPointF& point)
{
    const QPointF snapped = snapToGrid(point);
    return QPointF(std::clamp(snapped.x(), Board::PortZone, Board::Width - Board::PortZone),
                   std::clamp(snapped.y(), 0.0, Board::Height));
}

void Canvas::addPorts()
{
    for (int i = 0; i < Board::PortCount; ++i)
    {
        const double y = Board::FirstPortY + Board::PortSpacing * i - Board::PortHeight / 2;

        auto* input = new PortItem(QString("X[%1]").arg(i), PortItem::Direction::Input);
        input->setPos(0, y);
        graphicsScene->addItem(input);

        auto* output = new PortItem(QString("Y[%1]").arg(i), PortItem::Direction::Output);
        output->setPos(Board::Width - Board::PortZone, y);
        graphicsScene->addItem(output);
    }
}

// ---------------------------------------------------------------- tools

void Canvas::onToolChanged(ToolChanged callback)
{
    toolChanged = std::move(callback);
}

void Canvas::setTool(Tool newTool, const std::string& definitionId)
{
    cancelAction();
    tool = newTool;
    placeDefinitionId = newTool == Tool::Place ? definitionId : std::string();

    if (tool != Tool::Select) graphicsScene->clearSelection();
    if (tool == Tool::Place) createGhost(placeDefinitionId);

    updateCursor();
    if (toolChanged) toolChanged(tool, placeDefinitionId);
}

void Canvas::cancelAction()
{
    cancelWire();
    cancelCarry();
    removeGhost();
}

void Canvas::updateCursor()
{
    Qt::CursorShape shape = Qt::ArrowCursor;
    switch (tool)
    {
    case Tool::Select: shape = Qt::ArrowCursor; break;
    case Tool::Wire:   shape = Qt::CrossCursor; break;
    case Tool::Move:   shape = carried ? Qt::BlankCursor : Qt::SizeAllCursor; break;
    case Tool::Delete: shape = Qt::ForbiddenCursor; break;
    case Tool::Place:  shape = Qt::BlankCursor; break;   // the gate itself is the cursor
    }
    viewport()->setCursor(shape);
}

// ---------------------------------------------------------------- mouse / keys

void Canvas::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        setTool(Tool::Select);   // right-click unselects the tool (and cancels what it was doing)
        return;
    }
    if (event->button() != Qt::LeftButton)
    {
        QGraphicsView::mousePressEvent(event);
        return;
    }

    switch (tool)
    {
    case Tool::Select:
        QGraphicsView::mousePressEvent(event);   // Qt handles click-to-select
        break;
    case Tool::Wire:
        wireClick(event->pos());
        break;
    case Tool::Move:
        if (carried) drop();
        else pickUp(event->pos());
        break;
    case Tool::Delete:
        deleteAt(event->pos());
        break;
    case Tool::Place:
        if (ghost)
        {
            const std::string definitionId = placeDefinitionId;
            const QPointF position = ghost->pos();
            removeGhost();
            addGateAt(definitionId, position);
        }
        setTool(Tool::Move);
        break;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent* event)
{
    const QPointF scenePosition = mapToScene(event->pos());

    if (tool == Tool::Wire && drawingWire())
        updatePreview(scenePosition);
    else if (tool == Tool::Move && carried)
        carried->setPos(scenePosition + carryOffset);   // the item snaps/limits itself
    else if (tool == Tool::Place && ghost)
        moveGhost(scenePosition);
    else
        QGraphicsView::mouseMoveEvent(event);
}

void Canvas::mouseReleaseEvent(QMouseEvent* event)
{
    if (tool == Tool::Select)
        QGraphicsView::mouseReleaseEvent(event);   // other tools are click-based; releases don't matter
}

void Canvas::mouseDoubleClickEvent(QMouseEvent* event)
{
    // The first click of a double-click already arrived as a normal press.
    if (tool == Tool::Wire && event->button() == Qt::LeftButton && drawingWire())
    {
        if (PinItem* pin = pinAt(event->pos())) finishWireAtPin(pin);
        else finishWireLoose(mapToScene(event->pos()));
    }
    else if (tool == Tool::Select)
    {
        QGraphicsView::mouseDoubleClickEvent(event);
    }
    // Other tools ignore the second click so it doesn't act twice.
}

void Canvas::keyPressEvent(QKeyEvent* event)
{
    if (event->key() == Qt::Key_Escape)
        setTool(Tool::Select);
    else
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
    painter->fillRect(rect, outsideColour);
    painter->fillRect(board, boardColour);

    // Grid dots
    painter->setPen(QPen(gridColour, 1.5, Qt::SolidLine, Qt::RoundCap));
    const QRectF area = rect.intersected(board);
    const double left = std::floor(area.left() / Board::Grid) * Board::Grid;
    const double top = std::floor(area.top() / Board::Grid) * Board::Grid;
    QVector<QPointF> dots;
    for (double x = left; x <= area.right(); x += Board::Grid)
        for (double y = top; y <= area.bottom(); y += Board::Grid)
            dots.append(QPointF(x, y));
    painter->drawPoints(dots.data(), static_cast<int>(dots.size()));

    // Port strips (no-go zones)
    painter->fillRect(QRectF(0, 0, Board::PortZone, Board::Height), stripColour);
    painter->fillRect(QRectF(Board::Width - Board::PortZone, 0, Board::PortZone, Board::Height), stripColour);
}

// ---------------------------------------------------------------- wire tool

PinItem* Canvas::pinAt(const QPoint& viewPosition) const
{
    for (QGraphicsItem* item : items(viewPosition))
        if (auto* pin = qgraphicsitem_cast<PinItem*>(item)) return pin;
    return nullptr;
}

WireItem* Canvas::looseEndAt(const QPointF& scenePosition) const
{
    for (WireItem* wire : wires)
        if (!wire->endPin() && QLineF(wire->endPoint(), scenePosition).length() <= 8.0)
            return wire;
    return nullptr;
}

bool Canvas::drawingWire() const
{
    return wireStart || wireContinue;
}

QPointF Canvas::wireDrawStart() const
{
    return wireStart ? wireStart->scenePos() : wireContinue->endPoint();
}

void Canvas::wireClick(const QPoint& viewPosition)
{
    const QPointF scenePosition = mapToScene(viewPosition);
    PinItem* pin = pinAt(viewPosition);

    if (!drawingWire())
    {
        // Start from a pin, or continue a wire from its loose end.
        if (pin) wireStart = pin;
        else if (WireItem* wire = looseEndAt(scenePosition)) wireContinue = wire;
        else return;

        previewPath = graphicsScene->addPath(QPainterPath(), QPen(QColor(120, 120, 120), 2, Qt::DashLine));
        previewPath->setZValue(5);
        updatePreview(scenePosition);
        return;
    }

    if (pin) finishWireAtPin(pin);
    else
    {
        wireBends.push_back(constrainWirePoint(scenePosition));   // add a bend
        updatePreview(scenePosition);
    }
}

void Canvas::finishWireAtPin(PinItem* pin)
{
    if (pin == wireStart) return;

    if (wireContinue)
    {
        if (pin == wireContinue->startPin()) return;
        wireContinue->extend(wireBends, pin, {});
        attach(pin, wireContinue);
    }
    else
    {
        createWire(wireStart, pin, wireBends, {});
    }
    cancelWire();   // stays in the Wire tool, ready for the next one
}

void Canvas::finishWireLoose(const QPointF& scenePosition)
{
    const QPointF end = constrainWirePoint(scenePosition);
    if (!wireBends.empty() && wireBends.back() == end)
        wireBends.pop_back();   // the double-click's first click added this point as a bend

    if (wireBends.empty() && end == wireDrawStart())
    {
        cancelWire();           // nothing drawn
        return;
    }

    if (wireContinue) wireContinue->extend(wireBends, nullptr, end);
    else createWire(wireStart, nullptr, wireBends, end);
    cancelWire();
}

void Canvas::createWire(PinItem* startPin, PinItem* endPin, const std::vector<QPointF>& bends, const QPointF& looseEnd)
{
    project.addWire({ 0, 0 }, { 0, 0 });   // real ends are filled in by WireItem
    auto* wire = new WireItem(static_cast<int>(wires.size()), startPin, endPin, bends, looseEnd, project);
    graphicsScene->addItem(wire);
    wires.push_back(wire);
    attach(startPin, wire);
    attach(endPin, wire);
}

void Canvas::updatePreview(const QPointF& cursor)
{
    if (!previewPath) return;
    std::vector<QPointF> points{ wireDrawStart() };
    points.insert(points.end(), wireBends.begin(), wireBends.end());
    points.push_back(constrainWirePoint(cursor));
    previewPath->setPath(WireItem::route(points));
}

void Canvas::cancelWire()
{
    delete previewPath;
    previewPath = nullptr;
    wireStart = nullptr;
    wireContinue = nullptr;
    wireBends.clear();
}

// ---------------------------------------------------------------- move tool

void Canvas::pickUp(const QPoint& viewPosition)
{
    for (QGraphicsItem* item : items(viewPosition))   // topmost first
    {
        QGraphicsItem* top = item;                    // a pin or label means its gate/port
        while (top->parentItem()) top = top->parentItem();

        if (qgraphicsitem_cast<GateItem*>(top) || qgraphicsitem_cast<PortItem*>(top))
        {
            carried = top;
            carryOrigin = top->pos();
            carryOffset = top->pos() - mapToScene(viewPosition);
            updateCursor();
            return;
        }
    }
}

void Canvas::drop()
{
    carried = nullptr;
    updateCursor();
}

void Canvas::cancelCarry()
{
    if (carried) carried->setPos(carryOrigin);   // put it back
    carried = nullptr;
}

// ---------------------------------------------------------------- place tool

void Canvas::createGhost(const std::string& definitionId)
{
    const Definition* definition = project.findDefinition(definitionId);
    if (!definition) return;

    ghost = new QGraphicsRectItem(0, 0, definition->symbol.width, definition->symbol.height);
    ghost->setBrush(QColor(238, 243, 255));
    ghost->setPen(QPen(QColor(47, 125, 246), 1.5, Qt::DashLine));
    ghost->setOpacity(0.75);
    ghost->setZValue(10);
    auto* label = new QGraphicsSimpleTextItem(QString::fromStdString(definition->name), ghost);
    const QRectF textBox = label->boundingRect();
    label->setPos((ghost->rect().width() - textBox.width()) / 2, (ghost->rect().height() - textBox.height()) / 2);
    graphicsScene->addItem(ghost);

    moveGhost(mapToScene(viewport()->mapFromGlobal(QCursor::pos())));
}

void Canvas::moveGhost(const QPointF& scenePosition)
{
    const QSizeF size = ghost->rect().size();
    ghost->setPos(constrainGatePosition(scenePosition - QPointF(size.width() / 2, size.height() / 2), size));
}

void Canvas::removeGhost()
{
    delete ghost;
    ghost = nullptr;
}

void Canvas::addGateAt(const std::string& definitionId, const QPointF& position)
{
    Instance* instance = project.place(definitionId, { position.x(), position.y() });
    if (!instance || !instance->definition) return;

    auto* gate = new GateItem(static_cast<int>(gates.size()), *instance->definition, project);
    gate->setPos(position);
    graphicsScene->addItem(gate);
    gates.push_back(gate);
}

// ---------------------------------------------------------------- delete tool

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
    detach(wire->startPin(), wire);
    detach(wire->endPin(), wire);

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