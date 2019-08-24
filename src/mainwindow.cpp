#include <iostream>

#include <QAction>
#include <QString>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Chunk.h"
#include "Topology.h"
#include "Neighbourhood.h"

// Dear future me who knows to avoid tight coupling and other cool software engineering patterns: I'm sorry

constexpr int MainWindow::PLAY_DELAY;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui_(new Ui::MainWindow), tickTimer_(new QTimer(this)),
    // TODO this is just a default - allow customization of topology and neighbourhood type
    automaton_(new Automaton(new FixedTopology(20, 20),
        new MooreNeighbourhoodType(1))) {
  
  // set default automaton rules for Conway's Game of Life - TODO allow customization
  automaton_->ruleset().setBornWith(3, true);
  automaton_->ruleset().setSurvivesWith(2, true);
  automaton_->ruleset().setSurvivesWith(3, true);
  
  ui_->setupUi(this);
  QMainWindow::centralWidget()->layout()->setContentsMargins(0, 0, 0, 0); // make it flush
  
  scene_ = new AutomatonScene(*automaton_, this);
  ui_->graphics->setScene(scene_);
  
  connect(&automaton_->chunkArray(), &ChunkArray::chunkAdded, this, &MainWindow::addChunkGraphicsItem);
  connect(&automaton_->chunkArray(), &ChunkArray::chunkRemoved, this, &MainWindow::removeChunkGraphicsItem);
  
  connect(ui_->actionNextGeneration, &QAction::triggered, this, &MainWindow::nextGeneration);
  connect(ui_->actionPlay, &QAction::triggered, this, &MainWindow::play);
  connect(ui_->actionPause, &QAction::triggered, this, &MainWindow::pause);
  connect(ui_->actionReset, &QAction::triggered, this, &MainWindow::reset);
  
  ui_->actionPause->setEnabled(false);
  connect(tickTimer_, &QTimer::timeout, this, &MainWindow::nextGeneration);
  
  updateStatusBar();
  
  // Add one chunk at (0, 0) to ward off QGraphicsView weirdness
  automaton_->chunkArray().insertOrNoop(0, 0);
}

MainWindow::~MainWindow() {
  // We don't delete all the ChunkGraphicsItems here because when we delete automaton_, the ChunkArray is destructed,
  // which emits a chunkRemoved signal for each chunk, which is connected to removeChunkGraphicsItem, which deletes
  // all the items.
  tickTimer_->stop();
  delete tickTimer_;
  tickTimer_ = nullptr;
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
  auto* item = new ChunkGraphicsItem(automaton_->chunkArray().at(x, y));
  scene_->addItem(item);
  chunkGIMap_.emplace(xy, item);
}

void MainWindow::removeChunkGraphicsItem(int x, int y) {
  std::pair<int, int> xy(x, y);
  if (!chunkGIMap_.count(xy)) {
    // also bad + out of sync
    std::cerr << "MainWindow's map of ChunkGraphicsItems is out of sync with ChunkArray: trying to remove "
                 "ChunkGraphicsItem at (" << x << ", " << y << "), but there isn't one there!" << std::endl;
    return;
  }
  ChunkGraphicsItem* item = chunkGIMap_.at(xy);
  scene_->removeItem(item);
  delete item;
  chunkGIMap_.erase(xy);
}

void MainWindow::nextGeneration() {
  automaton_->tick();
  updateStatusBar();
}

void MainWindow::play() {
  tickTimer_->start(PLAY_DELAY);
  ui_->actionPlay->setEnabled(false);
  ui_->actionNextGeneration->setEnabled(false);
  ui_->actionPause->setEnabled(true);
}

void MainWindow::pause() {
  tickTimer_->stop();
  ui_->actionPause->setEnabled(false);
  ui_->actionPlay->setEnabled(true);
  ui_->actionNextGeneration->setEnabled(true);
}

void MainWindow::reset() {
  // Stop the timer, reset the automaton, and re-add chunk (0, 0) to avoid bugging out
  if (tickTimer_->isActive()) {
    pause();
  }
  automaton_->reset();
  automaton_->chunkArray().insertOrNoop(0, 0);
  updateStatusBar();
}

void MainWindow::updateStatusBar() const {
  statusBar()->showMessage(tr("Generation: ") + QString::number(automaton_->generation()));
}
