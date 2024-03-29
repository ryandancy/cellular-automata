#include <array>
#include <cstdio>
#include <iostream>
#include <memory>
#include <string>

#include <QAction>
#include <QDialog>
#include <QString>

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "Chunk.h"
#include "GraphicsProperties.h"
#include "Topology.h"
#include "Neighbourhood.h"
#include "NeighbourhoodDialog.h"
#include "RulesDialog.h"
#include "TopologyDialog.h"

// Dear future me who knows to avoid tight coupling and other cool software engineering patterns: I'm sorry

constexpr int MainWindow::MAX_PLAY_DELAY;

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent), ui_(new Ui::MainWindow), tickTimer_(new QTimer(this)),
    // default is unbounded topology with radius-1 Moore neighbourhood (Life setup)
    automaton_(new Automaton(new UnboundedTopology,
        new MooreNeighbourhoodType(1))) {
  
  // set default automaton rules for Conway's Game of Life - TODO add "presets" of rule sets
  automaton_->ruleset().setBornWith(3, true);
  automaton_->ruleset().setSurvivesWith(2, true);
  automaton_->ruleset().setSurvivesWith(3, true);
  
  ui_->setupUi(this);
  QMainWindow::centralWidget()->layout()->setContentsMargins(0, 0, 0, 0); // make it flush
  
  // for some reason, adding a widget to the toolbar can't be done in Qt Designer, so here we are
  speedSlider_ = new QSlider(Qt::Orientation::Horizontal, ui_->toolBar);
  speedSlider_->setMinimum(0);
  speedSlider_->setMaximum(1000);
  // this is the inversion of the function delay = MAX_PLAY_DELAY(1 - value/1000)^2 used to calculate position
  speedSlider_->setValue((int) (1000.0 * (1.0 - sqrt((double) playDelay_ / (double) MAX_PLAY_DELAY))));
  ui_->toolBar->insertWidget(ui_->actionPause, speedSlider_);
  connect(speedSlider_, &QSlider::valueChanged, this, &MainWindow::updatePlaySpeed);
  
  scene_ = new AutomatonScene(automaton_, this);
  ui_->graphics->setScene(scene_);
  connect(scene_, &AutomatonScene::cellUpdated, this, &MainWindow::updateStatusBar);
  
  connectAutomaton();
  
  connect(ui_->actionNextGeneration, &QAction::triggered, this, &MainWindow::nextGeneration);
  connect(ui_->actionPlay, &QAction::triggered, this, &MainWindow::play);
  connect(ui_->actionPause, &QAction::triggered, this, &MainWindow::pause);
  connect(ui_->actionReset, &QAction::triggered, this, &MainWindow::reset);
  connect(ui_->actionChangeRules, &QAction::triggered, this, &MainWindow::launchChangeRulesDialog);
  connect(ui_->actionChangeNeighbourhood, &QAction::triggered, this, &MainWindow::launchChangeNeighbourhoodTypeDialog);
  connect(ui_->actionChangeTopology, &QAction::triggered, this, &MainWindow::launchChangeTopologyDialog);
  connect(ui_->actionShowChunkBoundaries, &QAction::triggered, this, &MainWindow::toggleChunkBoxes);
  
  // make all the theme actions mutually exclusive via a QActionGroup
  themeGroup_ = new QActionGroup(this);
  themeGroup_->addAction(ui_->actionThemeLight);
  themeGroup_->addAction(ui_->actionThemeDark);
  themeGroup_->addAction(ui_->actionThemeHacker);
  themeGroup_->addAction(ui_->actionThemeCanadian);
  themeGroup_->addAction(ui_->actionThemeViolet);
  themeGroup_->setEnabled(true);
  
  connect(ui_->actionThemeLight, &QAction::triggered, this, [this] () {
    setTheme(GraphicsProperties::Theme::LIGHT);
  });
  connect(ui_->actionThemeDark, &QAction::triggered, this, [this] () {
    setTheme(GraphicsProperties::Theme::DARK);
  });
  connect(ui_->actionThemeHacker, &QAction::triggered, this, [this] () {
    setTheme(GraphicsProperties::Theme::HACKER);
  });
  connect(ui_->actionThemeCanadian, &QAction::triggered, this, [this] () {
    setTheme(GraphicsProperties::Theme::CANADIAN);
  });
  connect(ui_->actionThemeViolet, &QAction::triggered, this, [this] () {
    setTheme(GraphicsProperties::Theme::VIOLET);
  });

#if defined(__APPLE__) && defined(__MACH__)
  // Only on macOS - detect dark mode by calling system command, set to dark if in dark mode
  // https://stackoverflow.com/a/478960
  std::unique_ptr<FILE, decltype(&pclose)> pipe(popen("defaults read -g AppleInterfaceStyle", "r"), pclose);
  if (pipe) { // otherwise we can't open - default to light
    std::array<char, 128> buffer{};
    std::string output;
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
      output += buffer.data();
    }
    if (output == "Dark\n") {
      // cool it's in dark mode - set to dark theme
      ui_->actionThemeDark->trigger();
    }
  }
#endif
  
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
  delete speedSlider_;
  speedSlider_ = nullptr;
  delete themeGroup_;
  themeGroup_ = nullptr;
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
  tickTimer_->start(playDelay_);
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
  pauseIfRunning();
  automaton_->reset();
  automaton_->chunkArray().insertOrNoop(0, 0);
  updateStatusBar();
}

void MainWindow::updatePlaySpeed(int value) { // 0 <= value <= 1000
  // We decompose according to the following formula: delay = MAX_PLAY_DELAY(1 - value/1000)^2
  double valuePct = value / 1000.0;
  playDelay_ = (int) (MAX_PLAY_DELAY*(1-valuePct)*(1-valuePct));
  tickTimer_->setInterval(playDelay_);
}

void MainWindow::launchChangeRulesDialog() {
  pauseIfRunning();
  RulesDialog dialog(automaton_->ruleset(), this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::launchChangeNeighbourhoodTypeDialog() {
  pauseIfRunning();
  NeighbourhoodDialog dialog(automaton_->ruleset().getNeighbourhoodType(), automaton_, this);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::launchChangeTopologyDialog() {
  pauseIfRunning();
  TopologyDialog dialog(*automaton_, this);
  connect(&dialog, &TopologyDialog::automatonUpdated, this, &MainWindow::updateAutomaton);
  dialog.setModal(true);
  dialog.exec();
}

void MainWindow::updateAutomaton(Automaton* newAutomaton) {
  delete automaton_; // will remove all the ChunkGraphicsItems
  automaton_ = newAutomaton;
  scene_->updateAutomaton(automaton_);
  connectAutomaton(); // *must* go before inserting any chunks
  automaton_->chunkArray().insertOrNoop(0, 0); // prevent buggy behaviour when there's no chunks
}

void MainWindow::toggleChunkBoxes() {
  GraphicsProperties::instance().showChunkBoxes = !GraphicsProperties::instance().showChunkBoxes;
  scene_->update();
}

void MainWindow::setTheme(GraphicsProperties::Theme theme) {
  GraphicsProperties::instance().setTheme(theme);
  scene_->updateBackground();
  scene_->update();
}

void MainWindow::connectAutomaton() {
  connect(&automaton_->chunkArray(), &ChunkArray::chunkAdded, this, &MainWindow::addChunkGraphicsItem);
  connect(&automaton_->chunkArray(), &ChunkArray::chunkRemoved, this, &MainWindow::removeChunkGraphicsItem);
}

void MainWindow::pauseIfRunning() {
  if (tickTimer_->isActive()) {
    pause();
  }
}

void MainWindow::updateStatusBar() const {
  statusBar()->showMessage(
      tr("Generation:") + " " + QString::number(automaton_->generation()) + " | "
      + tr("Population:") + " " + QString::number(automaton_->population()));
}
