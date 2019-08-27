#include <QPushButton>

#include "Chunk.h"
#include "NeighbourhoodDialog.h"
#include "ui_neighbourhooddialog.h"

NeighbourhoodDialog::NeighbourhoodDialog(NeighbourhoodType& current, Automaton* automaton, QWidget* parent)
    : QDialog(parent), current_(current), ui_(new Ui::NeighbourhoodDialog), automaton_(automaton),
    typeGroup_(new QButtonGroup(this)) {
  ui_->setupUi(this);
  
  ui_->radiusSpinbox->setMaximum(CHUNK_SIZE-1); // above this probably won't work
  
  // make the radio buttons mutually exclusive
  typeGroup_->addButton(ui_->mooreButton);
  typeGroup_->addButton(ui_->vonNeumannButton);
  typeGroup_->setExclusive(true);
  
  if (dynamic_cast<MooreNeighbourhoodType*>(&current)) { // test if it's a Moore neighbourhood
    ui_->mooreButton->setChecked(true);
  } else { // there are only two neighbourhood types currently, so it's a von Neumann neighbourhood
    ui_->vonNeumannButton->setChecked(true);
  }
  
  ui_->radiusSpinbox->setValue(current.getRadius());
  
  connect(ui_->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(ui_->buttonBox, &QDialogButtonBox::accepted, this, &NeighbourhoodDialog::apply);
}

NeighbourhoodDialog::~NeighbourhoodDialog() {
  delete typeGroup_;
  typeGroup_ = nullptr;
  delete ui_;
  ui_ = nullptr;
}

void NeighbourhoodDialog::apply() {
  // construct the new neighbourhood type
  NeighbourhoodType* newType;
  int radius = ui_->radiusSpinbox->value();
  if (ui_->vonNeumannButton->isChecked()) {
    newType = new VonNeumannNeighbourhoodType(radius);
  } else { // Moore is the default if somehow neither are checked
    newType = new MooreNeighbourhoodType(radius);
  }
  
  automaton_->setNeighbourhoodType(newType);
  accept();
}
