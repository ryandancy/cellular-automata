#ifndef GAME_OF_LIFE_CHUNK_H
#define GAME_OF_LIFE_CHUNK_H

#include "Side.h"

#define CHUNK_SIZE 20

class Chunk {
public:
  void tick(); // move the entire Chunk to the next generation
  void tick(Side side); // like tick(), but only update the cells whose neighbourhoods run off on the given Side
  
private:
  bool cells[CHUNK_SIZE][CHUNK_SIZE];
};


#endif //GAME_OF_LIFE_CHUNK_H
