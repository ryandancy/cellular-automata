#ifndef GAME_OF_LIFE_AUTOMATON_H
#define GAME_OF_LIFE_AUTOMATON_H

#include <memory>

#include "Chunk.h"
#include "Ruleset.h"
#include "Neighbourhood.h"

// An Automaton encapsulates the entire cellular automaton. It owns a Topology*, a Ruleset, and
// a ChunkArray. It can advance the generation of the automaton by calling Automaton::tick().
// TODO some way to access the contents for displaying
class Automaton {
public:
  // Initialize the Automaton with a given topology (fixed) and neighbourhood type (can be modified later).
  // Take ownership of both pointers - the user must not delete them.
  // Throw std::invalid_argument if either pointer is null.
  Automaton(Topology* topology, NeighbourhoodType* initialNeighbourhoodType);
  
  // Set a new neighbourhood type. Take ownership of it, so the user mustn't delete it.
  // Throw std::invalid_argument if it is null.
  void setNeighbourhoodType(NeighbourhoodType* neighbourhoodType);
  
  // Advance the entire automaton to the next generation.
  void tick();
  
  // Get the current generation. The first generation is 0.
  int generation() const noexcept;
  
private:
  ChunkArray chunkArray_;
  std::shared_ptr<Topology> topology_; // TODO should this just be a unique_ptr owned by ChunkArray?
  Ruleset ruleset_;
  int generation_;
};

#endif //GAME_OF_LIFE_AUTOMATON_H
