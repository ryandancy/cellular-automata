#ifndef GAME_OF_LIFE_BOARD_H
#define GAME_OF_LIFE_BOARD_H

#include <memory>

#include "Chunk.h"
#include "Ruleset.h"
#include "Neighbourhood.h"

class Board {
public:
  Board(Topology* topology, NeighbourhoodType* initialNeighbourhoodType);
  void tick();
  
private:
  ChunkArray chunkArray_;
  std::shared_ptr<Topology> topology_;
  Ruleset ruleset_;
};

#endif //GAME_OF_LIFE_BOARD_H
