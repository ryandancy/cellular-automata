#ifndef GAME_OF_LIFE_NEIGHBOURHOODDIALOG_H
#define GAME_OF_LIFE_NEIGHBOURHOODDIALOG_H

#include <QButtonGroup>
#include <QDialog>
#include <QWidget>

#include "Automaton.h"
#include "Neighbourhood.h"

namespace Ui {
  class NeighbourhoodDialog;
}

// A dialog in which the user can choose and customize a neighbourhood type.
class NeighbourhoodDialog : public QDialog {
public:
  NeighbourhoodDialog(NeighbourhoodType& current, Automaton* automaton, QWidget* parent = nullptr);
  ~NeighbourhoodDialog() override;
  
private slots:
  void apply();
  
private:
  Ui::NeighbourhoodDialog* ui_;
  QButtonGroup* typeGroup_; // for making the neighbourhood type radio buttons mutually exclusive
  
  Automaton* automaton_; // to modify if the user clicks ok
  NeighbourhoodType& current_; // the current neighbourhood type
};

#endif //GAME_OF_LIFE_NEIGHBOURHOODDIALOG_H
