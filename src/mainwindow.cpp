#include <iostream>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Chunk.h"
#include "Topology.h"
#include "Neighbourhood.h"

// Dear future me who knows to avoid tight coupling and other cool software engineering patterns: I'm sorry

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui_(new Ui::MainWindow),
    // TODO this is just a default - allow customization of topology and neighbourhood type
    automaton_(new Automaton(new WrappingTopology(20, 20),
        new MooreNeighbourhoodType(1))) {
  
  ui_->setupUi(this);
  QMainWindow::centralWidget()->layout()->setContentsMargins(0, 0, 0, 0); // make it flush
  
  scene_ = new QGraphicsScene(this);
  ui_->graphics->setScene(scene_);
  
  connect(&automaton_->chunkArray(), &ChunkArray::chunkAdded, this, &MainWindow::addChunkGraphicsItem);
  connect(&automaton_->chunkArray(), &ChunkArray::chunkRemoved, this, &MainWindow::removeChunkGraphicsItem);
}

MainWindow::~MainWindow() {
  for (const std::pair<const std::pair<int, int>, ChunkGraphicsItem*>& element : chunkGIMap_) {
    // delete all the ChunkGraphicsItems
    delete element.second;
  }
  delete automaton_;
  automaton_ = nullptr;
  delete scene_;
  scene_ = nullptr;
  delete ui_;
  ui_ = nullptr;
}

void MainWindow::addChunkGraphicsItem(int x, int y) {
  std::pair<int, int> xy(x, y);
  if (chunkGIMap_.count(xy)) {
    // this is bad, chunkGIMap_ isn't properly sync'd with the ChunkArray
    std::cerr << "MainWindow's map of ChunkGraphicsItems is out of sync with ChunkArray: trying to add "
                 "ChunkGraphicsItem at (" << x << ", " << y << "), but it's already there! Removing and re-adding."
                 << std::endl;
    chunkGIMap_.erase(xy);
  }
  chunkGIMap_.emplace(xy, new ChunkGraphicsItem(automaton_->chunkArray().at(x, y)));
}

void MainWindow::removeChunkGraphicsItem(int x, int y) {
  std::pair<int, int> xy(x, y);
  if (!chunkGIMap_.count(xy)) {
    // also bad + out of sync
    std::cerr << "MainWindow's map of ChunkGraphicsItems is out of sync with ChunkArray: trying to remove "
                 "ChunkGraphicsItem at (" << x << ", " << y << "), but there isn't one there!" << std::endl;
    return;
  }
  delete chunkGIMap_.at(xy); // got to clean it up
  chunkGIMap_.erase(xy);
}
