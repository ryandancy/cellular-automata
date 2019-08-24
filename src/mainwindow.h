#ifndef GAME_OF_LIFE_MAINWINDOW_H
#define GAME_OF_LIFE_MAINWINDOW_H

#include <unordered_map>
#include <utility>

#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMainWindow>

#include "Automaton.h"
#include "ChunkGraphicsItem.h"
#include "util.h"

// TODO Separate UI from model via file structure, also figure out namespaces
// TODO Put the model on a separate thread from the UI
// TODO Allow customization of Topology + NeighbourhoodType

namespace Ui {
  class MainWindow; // Qt will fill this in from mainwindow.ui
}

class MainWindow : public QMainWindow {
  Q_OBJECT // Qt macro
  
public:
  explicit MainWindow(QWidget* parent = nullptr);
  ~MainWindow() override;
  
public slots:
  void addChunkGraphicsItem(int x, int y);
  void removeChunkGraphicsItem(int x, int y);
  
private:
  Ui::MainWindow* ui_;
  QGraphicsScene* scene_; // where we draw the actual automaton
  Automaton* automaton_; // the model itself
  
  // Analogous to ChunkArray's map, but for ChunkGraphicsItems
  std::unordered_map<std::pair<int, int>, ChunkGraphicsItem*, pair_hash> chunkGIMap_;
};

#endif //GAME_OF_LIFE_MAINWINDOW_H
