#include "00_MenuBar.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>

void buildHelpMenu(QMenuBar* menuBar)
{
    QMenu* menu = menuBar->addMenu("Help");
    menu->addAction("About");
}
