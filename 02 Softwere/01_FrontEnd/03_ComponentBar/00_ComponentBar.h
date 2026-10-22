#pragma once

class QToolBar;
class Project;
class Canvas;

// One button per part in every library. Clicking a button places that part on the canvas.
void buildComponentBar(QToolBar* bar, const Project& project, Canvas* canvas);
