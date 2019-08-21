#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), mainWindow(new Ui::MainWindow) {}

MainWindow::~MainWindow() {
  delete mainWindow;
}
