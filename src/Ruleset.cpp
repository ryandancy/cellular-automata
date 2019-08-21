#include <algorithm>
#include <stdexcept>

#include "Ruleset.h"

// Main constructor
Ruleset::Ruleset(NeighbourhoodType* neighbourhoodType) : neighbourhoodType_(neighbourhoodType) {
  unsigned int arrSize = neighbourhoodType_->getNumCells() + 1;
  born_ = new bool[arrSize](); // set both to all zeroes; apparently, the parentheses does that
  survive_ = new bool[arrSize]();
}

// Destructor
Ruleset::~Ruleset() {
  delete neighbourhoodType_;
  neighbourhoodType_ = nullptr;
  
  delete[] born_;
  born_ = nullptr;
  
  delete[] survive_;
  survive_ = nullptr;
}

// Copy constructor
Ruleset::Ruleset(Ruleset& toCopy) {
  neighbourhoodType_ = toCopy.neighbourhoodType_->clone();
  
  unsigned int arrSize = toCopy.neighbourhoodType_->getNumCells() + 1;
  born_ = new bool[arrSize];
  survive_ = new bool[arrSize];
  
  std::copy(toCopy.born_, toCopy.born_ + arrSize, born_);
  std::copy(toCopy.survive_, toCopy.survive_ + arrSize, survive_);
}

// Copy assignment operator
Ruleset& Ruleset::operator=(const Ruleset& toCopy) {
  delete neighbourhoodType_;
  neighbourhoodType_ = toCopy.neighbourhoodType_->clone();
  
  unsigned int arrSize = toCopy.neighbourhoodType_->getNumCells() + 1;
  
  delete[] born_;
  born_ = new bool[arrSize];
  
  delete[] survive_;
  survive_ = new bool[arrSize];
  
  std::copy(toCopy.born_, toCopy.born_ + arrSize, born_);
  std::copy(toCopy.survive_, toCopy.survive_ + arrSize, survive_);
  
  return *this;
}

// Move constructor
Ruleset::Ruleset(Ruleset&& toMove) noexcept {
  neighbourhoodType_ = toMove.neighbourhoodType_;
  born_ = toMove.born_;
  survive_ = toMove.survive_;
  
  toMove.neighbourhoodType_ = nullptr;
  toMove.born_ = nullptr;
  toMove.survive_ = nullptr;
}

// Move assignment operator
Ruleset& Ruleset::operator=(Ruleset&& toMove) noexcept {
  delete neighbourhoodType_;
  delete[] born_;
  delete[] survive_;
  
  neighbourhoodType_ = toMove.neighbourhoodType_;
  born_ = toMove.born_;
  survive_ = toMove.survive_;
  
  toMove.neighbourhoodType_ = nullptr;
  toMove.born_ = nullptr;
  toMove.survive_ = nullptr;
  
  return *this;
}

// NeighbourhoodType manipulation

NeighbourhoodType& Ruleset::getNeighbourhoodType() {
  return *neighbourhoodType_;
}

void Ruleset::setNeighbourhoodType(NeighbourhoodType* neighbourhoodType) {
  // Take ownership of neighbourhoodType and reinitialize born_ and survive_
  delete neighbourhoodType_;
  delete[] born_;
  delete[] survive_;
  
  neighbourhoodType_ = neighbourhoodType;
  born_ = survive_ = nullptr; // leave both valid in case the first born_ initialization throws
  
  unsigned int arrSize = neighbourhoodType_->getNumCells() + 1;
  born_ = new bool[arrSize]();
  survive_ = new bool[arrSize]();
}

// Retrieving and setting rules

bool Ruleset::isBornWith(unsigned int numNeighbours) const {
  checkNumNeighboursInRange(numNeighbours);
  return born_[numNeighbours];
}

bool Ruleset::survivesWith(unsigned int numNeighbours) const {
  checkNumNeighboursInRange(numNeighbours);
  return survive_[numNeighbours];
}

void Ruleset::setBornWith(unsigned int numNeighbours, bool value) {
  checkNumNeighboursInRange(numNeighbours);
  born_[numNeighbours] = value;
}

void Ruleset::setSurvivesWith(unsigned int numNeighbours, bool value) {
  checkNumNeighboursInRange(numNeighbours);
  survive_[numNeighbours] = value;
}

// Utility

void Ruleset::checkNumNeighboursInRange(unsigned int numNeighbours) const {
  if (numNeighbours > neighbourhoodType_->getNumCells()) {
    throw std::invalid_argument("Cannot set or get rule for number of neighbours greater than "
        "the total number of cells in the neighbourhood.");
  }
}
