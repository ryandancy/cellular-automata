#include <QString>

#include "RulesDialog.h"
#include "ui_rulesdialog.h"

constexpr unsigned int RulesDialog::CHECKBOXES_PER_ROW;

RulesDialog::RulesDialog(Ruleset& ruleset, QWidget* parent) : QDialog(parent), ui_(new Ui::RulesDialog),
    bornGrid_(new QGridLayout), survivesGrid_(new QGridLayout), ruleset_(ruleset) {
  ui_->setupUi(this);
  
  // add born/survives checkboxes
  unsigned int numCells = ruleset_.getNeighbourhoodType().getNumCells();
  for (unsigned int numNeighbours = 1; numNeighbours <= numCells; numNeighbours++) {
    auto bornCheckbox = new QCheckBox(QString::number(numNeighbours), this);
    auto survivesCheckbox = new QCheckBox(QString::number(numNeighbours), this);
    
    connect(bornCheckbox, &QCheckBox::stateChanged, this, [this, numNeighbours] (int value) {
      setBornRule(numNeighbours, value == Qt::CheckState::Checked);
    });
    connect(survivesCheckbox, &QCheckBox::stateChanged, this, [this, numNeighbours] (int value) {
      setSurvivesRule(numNeighbours, value == Qt::CheckState::Checked);
    });
    
    bornCheckbox->setChecked(ruleset_.isBornWith(numNeighbours));
    survivesCheckbox->setChecked(ruleset_.survivesWith(numNeighbours));
    
    unsigned int x = (numNeighbours - 1) % CHECKBOXES_PER_ROW;
    unsigned int y = (numNeighbours - 1) / CHECKBOXES_PER_ROW;
    
    bornGrid_->addWidget(bornCheckbox, (int) y, (int) x); // row, then column
    survivesGrid_->addWidget(survivesCheckbox, (int) y, (int) x);
    
    bornCheckboxes_.push_back(bornCheckbox);
    survivesCheckBoxes_.push_back(survivesCheckbox);
  }
  
  ui_->bornGroupBox->setLayout(bornGrid_);
  ui_->survivesGroupBox->setLayout(survivesGrid_);
}

RulesDialog::~RulesDialog() {
  for (QCheckBox* checkbox : bornCheckboxes_) {
    delete checkbox;
  }
  for (QCheckBox* checkbox : survivesCheckBoxes_) {
    delete checkbox;
  }
  delete bornGrid_;
  bornGrid_ = nullptr;
  delete survivesGrid_;
  survivesGrid_ = nullptr;
  delete ui_;
  ui_ = nullptr;
}

void RulesDialog::setBornRule(unsigned int numNeighbours, bool value) {
  ruleset_.setBornWith(numNeighbours, value);
}

void RulesDialog::setSurvivesRule(unsigned int numNeighbours, bool value) {
  ruleset_.setSurvivesWith(numNeighbours, value);
}
