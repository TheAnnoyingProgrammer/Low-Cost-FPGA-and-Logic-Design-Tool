#include "00_MenuBar.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>

void buildToolsMenu(QMenuBar* menuBar)
{
    QMenu* menu = menuBar->addMenu("Tools");
    menu->addAction("Placeholder Tool");
}
