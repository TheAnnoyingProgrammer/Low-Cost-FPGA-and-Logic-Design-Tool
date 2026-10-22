#pragma once

#include <QGraphicsView>
#include <string>
#include <vector>

class Project;
class GateItem;
class PinItem;
class WireItem;
class QAction;
class QGraphicsPathItem;

// Fixed board size and layout (scene units).
namespace Board
{
constexpr double Width = 1200.0;
constexpr double Height = 800.0;
constexpr double Grid = 10.0;          // snap size
constexpr double PortWidth = 50.0;
constexpr double PortHeight = 20.0;
constexpr double PortSpacing = 40.0;
constexpr double FirstPortY = 100.0;
constexpr int PortCount = 16;          // per side
constexpr double PortZone = 70.0;      // gates can't enter this strip on the left/right
}

// The drawing area: a fixed board with FPGA ports on the left (IN) and right (OUT).
//  - Drag a gate to move it (snaps to grid, stays on the board).
//  - Press on a pin and drag to another pin to connect a wire.
//  - Delete mode: click a gate or wire to remove it. Esc leaves delete mode.
class Canvas : public QGraphicsView
{
public:
    explicit Canvas(Project& project, QWidget* parent = nullptr);

    void addGate(const std::string& definitionId);
    void setDeleteAction(QAction* action);
    void setDeleteMode(bool on);

    static QPointF constrainGatePosition(const QPointF& position, const QSizeF& size);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void drawBackground(QPainter* painter, const QRectF& rect) override;

private:
    void addPorts();
    PinItem* pinAt(const QPoint& viewPosition) const;
    void connectPins(PinItem* startPin, PinItem* endPin);
    void cancelWire();
    void deleteAt(const QPoint& viewPosition);
    void deleteGate(GateItem* gate);
    void deleteWire(WireItem* wire);

    Project& project;
    QGraphicsScene* graphicsScene;

    // Kept in the same order as project.instances / project.wires.
    std::vector<GateItem*> gates;
    std::vector<WireItem*> wires;

    PinItem* wireStart = nullptr;
    QGraphicsPathItem* previewPath = nullptr;

    bool deleteMode = false;
    QAction* deleteAction = nullptr;
    int placedCount = 0;
};