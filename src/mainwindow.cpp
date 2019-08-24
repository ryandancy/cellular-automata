#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui(new Ui::MainWindow) {
  ui->setupUi(this);
  QMainWindow::centralWidget()->layout()->setContentsMargins(0, 0, 0, 0); // make it flush
  
  scene = new QGraphicsScene(this);
  ui->graphics->setScene(scene);
}

MainWindow::~MainWindow() {
  delete scene;
  scene = nullptr;
  delete ui;
  ui = nullptr;
}
