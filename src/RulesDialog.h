#ifndef GAME_OF_LIFE_RULESDIALOG_H
#define GAME_OF_LIFE_RULESDIALOG_H

#include <vector>

#include <QCheckBox>
#include <QDialog>
#include <QGridLayout>
#include <QWidget>

#include "Ruleset.h"

namespace Ui {
  class RulesDialog;
}

// A dialog which allows the user to change the automaton's rules. It has two groups of checkboxes, one for the rules
// on cells being born, and one for the rules on cells surviving.
class RulesDialog : public QDialog {
  Q_OBJECT
  
public:
  explicit RulesDialog(Ruleset& ruleset, QWidget* parent = nullptr); // initialize with the given ruleset
  ~RulesDialog() override;
  
private:
  // Set the rule for born or survives with the given number of neighbours to the given value. Throw
  // std::invalid_argument if numNeighbours > the number of neighbours in the neighbourhood.
  // These could be slots, but they're never connected to precisely as such, so they aren't.
  void setBornRule(unsigned int numNeighbours, bool value);
  void setSurvivesRule(unsigned int numNeighbours, bool value);
  
  static constexpr unsigned int CHECKBOXES_PER_ROW = 4;
  
  Ui::RulesDialog* ui_;
  Ruleset& ruleset_;
  
  QGridLayout* bornGrid_; // the layouts for the check boxes
  QGridLayout* survivesGrid_;
  
  std::vector<QCheckBox*> bornCheckboxes_; // these hold the check boxes
  std::vector<QCheckBox*> survivesCheckBoxes_;
};

#endif //GAME_OF_LIFE_RULESDIALOG_H
