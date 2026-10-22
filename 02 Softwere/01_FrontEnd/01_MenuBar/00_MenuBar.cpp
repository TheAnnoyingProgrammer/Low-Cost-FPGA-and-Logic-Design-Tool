#include "00_MenuBar.h"

void buildMenuBar(QMenuBar* menuBar)
{
    buildFileMenu(menuBar);
    buildEditMenu(menuBar);
    buildToolsMenu(menuBar);
    buildHelpMenu(menuBar);
}
