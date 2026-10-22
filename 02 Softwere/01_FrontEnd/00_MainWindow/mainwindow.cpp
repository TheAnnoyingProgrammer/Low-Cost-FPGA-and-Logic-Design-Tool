#include "mainwindow.h"
#include "../01_MenuBar/00_MenuBar.h"
#include "../02_Canvas/00_Canvas.h"
#include "../03_ComponentBar/00_ComponentBar.h"
#include <QToolBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , canvas(new Canvas(project, this))
{
    setWindowTitle("BuddyBoard IDE");
    buildMenuBar(menuBar());

    setCentralWidget(canvas);

    QToolBar* componentBar = addToolBar("Components");
    buildComponentBar(componentBar, project, canvas);
}

MainWindow::~MainWindow()
{
}