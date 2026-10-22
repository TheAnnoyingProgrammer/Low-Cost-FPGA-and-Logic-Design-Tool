#include "mainwindow.h"
#include "../01_MenuBar/00_MenuBar.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("BuddyBoard IDE");
    buildMenuBar(menuBar());
}

MainWindow::~MainWindow()
{
}
