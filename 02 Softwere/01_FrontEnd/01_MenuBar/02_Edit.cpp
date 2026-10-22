#include "00_MenuBar.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>

void buildEditMenu(QMenuBar* menuBar)
{
    QMenu* menu = menuBar->addMenu("Edit");
    menu->addAction("Undo");
    menu->addAction("Redo");
    menu->addSeparator();
    menu->addAction("Cut");
    menu->addAction("Copy");
    menu->addAction("Paste");
}
