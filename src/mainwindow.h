#ifndef GAME_OF_LIFE_MAINWINDOW_H
#define GAME_OF_LIFE_MAINWINDOW_H

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMainWindow>

// TODO Separate UI from model via file structure, also figure out namespaces
// TODO Put the model on a separate thread from the UI

namespace Ui {
  class MainWindow; // Qt will fill this in from mainwindow.ui
}

class MainWindow : public QMainWindow {
  Q_OBJECT // Qt macro
  
public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;
  
private:
  Ui::MainWindow* ui;
  QGraphicsScene* scene; // where we draw the actual automaton
};

#endif //GAME_OF_LIFE_MAINWINDOW_H
