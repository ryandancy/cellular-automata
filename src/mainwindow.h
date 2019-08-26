#ifndef GAME_OF_LIFE_MAINWINDOW_H
#define GAME_OF_LIFE_MAINWINDOW_H

#include <unordered_map>
#include <utility>

#include <QGraphicsItem>
#include <QMainWindow>
#include <QSlider>
#include <QTimer>
#include <QWidget>

#include "Automaton.h"
#include "AutomatonScene.h"
#include "ChunkGraphicsItem.h"
#include "util.h"

// TODO Separate UI from model via file structure, also figure out namespaces
// TODO Put the model on a separate thread from the UI
// TODO Allow customization of Topology + NeighbourhoodType
// TODO Add "painting" live cells
// TODO Allow scrolling beyond edge of ChunkGraphicsItems - https://stackoverflow.com/q/19207153?

namespace Ui {
  class MainWindow; // Qt will fill this in from mainwindow.ui
}

// The main window of the application.
class MainWindow : public QMainWindow {
  Q_OBJECT // Qt macro
  
public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;
  
public slots:
  void addChunkGraphicsItem(int x, int y);
  void removeChunkGraphicsItem(int x, int y);
  
private slots:
  void nextGeneration();
  void play();
  void pause();
  void reset();
  void updatePlaySpeed(int value);
  
  void launchChangeRulesDialog();
  
  void toggleChunkBoxes();
  
private:
  void updateStatusBar() const; // Update "Generation: X" in the status bar
  
  static constexpr int MAX_PLAY_DELAY = 400; // min is 0; time in ms between updates when playing
  int playDelay_ = 200;
  
  Ui::MainWindow* ui_;
  AutomatonScene* scene_; // where we draw the actual automaton
  Automaton* automaton_; // the model itself
  QTimer* tickTimer_; // for playing
  QSlider* speedSlider_; // for controlling play speed
  
  // Analogous to ChunkArray's map, but for ChunkGraphicsItems
  std::unordered_map<std::pair<int, int>, ChunkGraphicsItem*, pair_hash> chunkGIMap_;
};

#endif //GAME_OF_LIFE_MAINWINDOW_H
