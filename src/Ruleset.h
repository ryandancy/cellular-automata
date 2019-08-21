#ifndef GAME_OF_LIFE_RULESET_H
#define GAME_OF_LIFE_RULESET_H

#include "Neighbourhood.h"

// A Ruleset represents the rules of the current cellular automaton, including the neighbourhood type and whether
// a dead cell is born (becomes alive), and whether a live cell survives, with each number of live neighbours.
// For the standard Conway's Game of Life, pass in a new MooreNeighbourhoodType, and call setBornWith(3, true),
// setSurvivesWith(2, true), and setSurvivesWith(3, true).
// This class owns a NeighbourhoodType* passed in during the constructor, as well as two dynamic bool arrays
// representing whether a cell is born or survives with each number of neighbours.
class Ruleset {
public:
  // Initialize the Ruleset with a NeighbourhoodType*. This Ruleset takes ownership of the NeighbourhoodType*
  // and so the object pointed to must not be deleted except by this class. Initialize the born and survive rules
  // to false.
  explicit Ruleset(NeighbourhoodType* neighbourhoodType);
  
  // Rule of Five destructor and copy and move constructors and assignment operators.
  // Needed for management of the NeighbourhoodType* and dynamic bool arrays.
  ~Ruleset();
  Ruleset(Ruleset& toCopy);
  Ruleset& operator=(const Ruleset& toCopy);
  Ruleset(Ruleset&& toMove) noexcept;
  Ruleset& operator=(Ruleset&& toMove) noexcept;
  
  // NeighbourhoodType management
  
  // Retrieve the NeighbourhoodType passed in, as a reference. Callers must not delete or (somehow) modify it.
  NeighbourhoodType& getNeighbourhoodType();
  
  // Set the NeighbourhoodType. Destroy the old NeighbourhoodType and take ownership of this one.
  // Reinitialize the born and survive rules to false.
  void setNeighbourhoodType(NeighbourhoodType* neighbourhoodType);
  
  // Retrieving and setting rules
  
  // Should a dead cell with this number of live neighbours become alive?
  // Throw std::invalid_argument if numNeighbours is greater than the total number of cells of the neighbourhood.
  bool isBornWith(unsigned int numNeighbours) const;
  
  // Should a live cell with this number of live neighbours survive?
  // Throw std::invalid_argument if numNeighbours is greater than the total number of cells of the neighbourhood.
  bool survivesWith(unsigned int numNeighbours) const;
  
  // Set whether a dead cell with this number of live neighbours becomes alive.
  // Throw std::invalid_argument if numNeighbours is greater than the total number of cells of the neighbourhood.
  void setBornWith(unsigned int numNeighbours, bool value);
  
  // Set whether a live cell with this number of live neighbours survives.
  // Throw std::invalid_argument if numNeighbours is greater than the total number of cells of the neighbourhood.
  void setSurvivesWith(unsigned int numNeighbours, bool value);
  
private:
  // Check that numNeighbours <= total number of cells of neighbourhood, throw std::invalid_argument otherwise.
  void checkNumNeighboursInRange(unsigned int numNeighbours) const;
  
  // Note: we *probably* should use smart pointers, but I want to implement the memory management, so whatever.
  
  // The NeighbourhoodType owned by this class.
  NeighbourhoodType* neighbourhoodType_;
  
  // These arrays are the same size as neighbourhoodType_->getNumCells() + 1.
  bool* born_; // born_[i]: should a dead cell with i live neighbours become alive?
  bool* survive_; // survive_[i]: should a live cell with i live neighbours survive?
};

#endif //GAME_OF_LIFE_RULESET_H
