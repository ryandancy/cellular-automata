#ifndef GAME_OF_LIFE_TOPOLOGYDIALOG_H
#define GAME_OF_LIFE_TOPOLOGYDIALOG_H

#include <QDialog>
#include <QWidget>

#include "Automaton.h"
#include "AutomatonScene.h"

namespace Ui {
  class TopologyDialog;
}

// A dialog which can change the topology of the field.
class TopologyDialog : public QDialog {
  Q_OBJECT
  
public:
  // We modify, but do not own, the Automaton referenced here
  explicit TopologyDialog(Automaton& automaton, QWidget* parent = nullptr);
  ~TopologyDialog() override;
  
signals:
  void automatonUpdated(Automaton* newAutomaton);
  
private slots:
  void apply(); // change the topology and exit
  
private:
  Ui::TopologyDialog* ui_;
  
  Automaton& automaton_; // the automaton whose topology we're changing, to steal the Ruleset from
};

#endif //GAME_OF_LIFE_TOPOLOGYDIALOG_H
