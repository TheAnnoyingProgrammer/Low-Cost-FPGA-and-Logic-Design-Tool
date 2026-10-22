#include "00_MenuBar.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>

void buildFileMenu(QMenuBar* menuBar)
{
    QMenu* menu = menuBar->addMenu("File");
    menu->addAction("New");
    menu->addAction("Open");
    menu->addAction("Save");
    menu->addAction("Save As");
    menu->addSeparator();
    menu->addAction("Exit");
}
