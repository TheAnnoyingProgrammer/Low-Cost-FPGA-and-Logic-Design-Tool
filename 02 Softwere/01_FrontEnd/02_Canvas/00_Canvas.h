#pragma once

#include <QGraphicsView>
#include <functional>
#include <string>
#include <vector>

class Project;
class GateItem;
class PinItem;
class WireItem;
class QGraphicsPathItem;
class QGraphicsRectItem;

namespace Board
{
constexpr double Width = 1900.0;
constexpr double Height = 950.0;
constexpr double Grid = 10.0;
constexpr double PortZone = 60.0;
constexpr double PortHeight = 20.0;
constexpr double PortSpacing = 20.0;
constexpr double FirstPortY = 330.0;
constexpr int PortCount = 16;
constexpr double GateMargin = 20.0;
}

class Canvas : public QGraphicsView
{
public:
    enum class Tool { Select, Wire, Move, Delete, Place };
    using ToolChanged = std::function<void(Tool, const std::string&)>;

    explicit Canvas(Project& project, QWidget* parent = nullptr);

    void setTool(Tool tool, const std::string& definitionId = {});
    Tool currentTool() const { return tool; }
    void onToolChanged(ToolChanged callback);

    static QPointF snapToGrid(const QPointF& point);
    static QPointF constrainGatePosition(const QPointF& position, const QSizeF& size);
    static QPointF constrainWirePoint(const QPointF& point);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void mouseDoubleClickEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void addPorts();
    void updateCursor();
    void cancelAction();

    PinItem* pinAt(const QPoint& viewPosition) const;
    WireItem* looseEndAt(const QPointF& scenePosition) const;
    bool drawingWire() const;
    QPointF wireDrawStart() const;
    void wireClick(const QPoint& viewPosition);
    void finishWireAtPin(PinItem* pin);
    void finishWireLoose(const QPointF& scenePosition);
    void createWire(PinItem* startPin, PinItem* endPin, const std::vector<QPointF>& bends, const QPointF& looseEnd);
    void updatePreview(const QPointF& cursor);
    void cancelWire();

    void pickUp(const QPoint& viewPosition);
    void drop();
    void cancelCarry();

    void createGhost(const std::string& definitionId);
    void moveGhost(const QPointF& scenePosition);
    void removeGhost();
    void addGateAt(const std::string& definitionId, const QPointF& position);

    void deleteAt(const QPoint& viewPosition);
    void deleteGate(GateItem* gate);
    void deleteWire(WireItem* wire);

    Project& project;
    QGraphicsScene* graphicsScene;
    Tool tool = Tool::Select;
    ToolChanged toolChanged;

    std::vector<GateItem*> gates;
    std::vector<WireItem*> wires;

    PinItem* wireStart = nullptr;
    WireItem* wireContinue = nullptr;
    std::vector<QPointF> wireBends;
    QGraphicsPathItem* previewPath = nullptr;

    QGraphicsItem* carried = nullptr;
    QPointF carryOffset;
    QPointF carryOrigin;

    std::string placeDefinitionId;
    QGraphicsRectItem* ghost = nullptr;
};