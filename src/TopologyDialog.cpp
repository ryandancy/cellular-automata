#include <utility>

#include <QAbstractButton>
#include <QDialogButtonBox>
#include <QSpinBox>

#include "Chunk.h"
#include "Ruleset.h"
#include "Topology.h"
#include "TopologyDialog.h"
#include "ui_topologydialog.h"

// Note: much of the complexity here comes from us displaying the width/height to the user in cells, while
// processing it in chunks, and so we have to fiddle with a factor of CHUNK_SIZE.

// we use a pointer-to-pointer to update the pointer in MainWindow
TopologyDialog::TopologyDialog(Automaton& automaton, QWidget* parent) : QDialog(parent),
    automaton_(automaton), ui_(new Ui::TopologyDialog) {
  
  ui_->setupUi(this);
  
  // make sure the spinboxes are up to date with CHUNK_SIZE
  ui_->widthSpinbox->setMinimum(CHUNK_SIZE);
  ui_->widthSpinbox->setSingleStep(CHUNK_SIZE);
  ui_->widthSpinbox->setMaximum(1000*CHUNK_SIZE);
  ui_->heightSpinbox->setMinimum(CHUNK_SIZE);
  ui_->heightSpinbox->setSingleStep(CHUNK_SIZE);
  ui_->heightSpinbox->setMaximum(1000*CHUNK_SIZE);
  
  Topology& currentTopology = automaton_.topology();
  
  // check the correct radio button, setup UI for the current topology
  if (dynamic_cast<UnboundedTopology*>(&currentTopology)) { // is it an UnboundedTopology?
    ui_->unboundedRadioButton->setChecked(true);
    ui_->widthSpinbox->setEnabled(false);
    ui_->heightSpinbox->setEnabled(false);
  } else if (dynamic_cast<FixedTopology*>(&currentTopology)) { // is it a FixedTopology?
    ui_->fixedRadioButton->setChecked(true);
  } else { // must be a WrappingTopology, it's the only concrete one left
    ui_->wrappingRadioButton->setChecked(true);
  }
  
  int initialWidth, initialHeight;
  if (currentTopology.bounded()) {
    initialWidth = currentTopology.width() * CHUNK_SIZE;
    initialHeight = currentTopology.height() * CHUNK_SIZE;
  } else {
    initialWidth = initialHeight = CHUNK_SIZE;
  }
  
  ui_->widthSpinbox->setValue(initialWidth);
  ui_->heightSpinbox->setValue(initialHeight);
  
  // make the radio buttons toggle enabling the spinboxes
  connect(ui_->unboundedRadioButton, &QAbstractButton::toggled, ui_->widthSpinbox, &QSpinBox::setDisabled);
  connect(ui_->unboundedRadioButton, &QAbstractButton::toggled, ui_->heightSpinbox, &QSpinBox::setDisabled);
  
  connect(ui_->buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
  connect(ui_->buttonBox, &QDialogButtonBox::accepted, this, &TopologyDialog::apply);
}

TopologyDialog::~TopologyDialog() {
  delete ui_;
  ui_ = nullptr;
}

void TopologyDialog::apply() {
  // make the new topology
  int width = ui_->widthSpinbox->value() / CHUNK_SIZE; // in chunks
  int height = ui_->heightSpinbox->value() / CHUNK_SIZE;
  Topology* newTopology;
  if (ui_->fixedRadioButton->isChecked()) {
    newTopology = new FixedTopology(width, height);
  } else if (ui_->wrappingRadioButton->isChecked()) {
    newTopology = new WrappingTopology(width, height);
  } else { // UnboundedTopology; it's the default
    newTopology = new UnboundedTopology;
  }
  
  // update the automaton using the old automaton's ruleset
  emit automatonUpdated(new Automaton(newTopology, std::move(automaton_.ruleset())));
  
  // substitute the automaton
//  delete *automaton_;
//  *automaton_ = new Automaton(newTopology, std::move(ruleset));
  
  // we invalidated the automaton pointer in the AutomatonScene when we deleted *automaton_ - update it
//  scene_.updateAutomaton(*automaton_);
  
  // close the window
  accept();
}
