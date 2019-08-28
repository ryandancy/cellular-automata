#ifndef GAME_OF_LIFE_AUTOMATON_H
#define GAME_OF_LIFE_AUTOMATON_H

#include <memory>

#include "Chunk.h"
#include "Ruleset.h"
#include "Neighbourhood.h"

// An Automaton encapsulates the entire cellular automaton. It owns a Topology*, a Ruleset, and
// a ChunkArray. It can advance the generation of the automaton by calling Automaton::tick().
class Automaton {
public:
  // Initialize the Automaton with a given topology (fixed) and neighbourhood type (can be modified later).
  // Take ownership of both pointers - the user must not delete them.
  // Throw std::invalid_argument if either pointer is null.
  Automaton(Topology* topology, NeighbourhoodType* initialNeighbourhoodType);
  
  // Initialize the Automaton with a given topology (fixed) and by moving from the given ruleset.
  // Take ownership of the topology. Throw std::invalid_argument if the pointer is null.
  Automaton(Topology* topology, Ruleset&& ruleset);
  
  // Set a new neighbourhood type. Take ownership of it, so the user mustn't delete it.
  // Throw std::invalid_argument if it is null.
  void setNeighbourhoodType(NeighbourhoodType* neighbourhoodType);
  
  // Advance the entire automaton to the next generation.
  void tick();
  
  // Reset the entire automaton. Remove all Chunks and reset the generation count.
  void reset();
  
  // Get the current generation. The first generation is 0.
  int generation() const noexcept;
  
  // Get the chunk array behind the automaton.
  ChunkArray& chunkArray() noexcept;
  
  // Get the Ruleset managing the automaton. (What's an encapsulation? Why not just have them be public?)
  Ruleset& ruleset() noexcept;
  
  // Get the Topology of the automaton.
  Topology& topology() noexcept;
  
private:
  // Call the appropriate generation functions for the given chunk, which is assumed to be empty.
  void generateEmptyChunk(int x, int y, Chunk& chunk, Neighbourhood& neighbourhood, int affectingDistance);
  
  ChunkArray chunkArray_;
  Ruleset ruleset_;
  int generation_ = 0;
};

#endif //GAME_OF_LIFE_AUTOMATON_H
