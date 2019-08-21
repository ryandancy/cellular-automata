#ifndef GAME_OF_LIFE_MAINWINDOW_H
#define GAME_OF_LIFE_MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
  class MainWindow; // Qt will fill this in from mainwindow.ui
}

class MainWindow : public QMainWindow {
  Q_OBJECT // Qt macro
  
public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;
  
private:
  Ui::MainWindow* mainWindow;
};

#endif //GAME_OF_LIFE_MAINWINDOW_H
