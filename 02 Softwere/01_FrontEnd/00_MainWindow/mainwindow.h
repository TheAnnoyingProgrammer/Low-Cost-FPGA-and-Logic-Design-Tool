#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "02_BackEnd/00_ClassStructures/00_Project.h"

class Canvas;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Project project;
    Canvas* canvas;
};

#endif // MAINWINDOW_H