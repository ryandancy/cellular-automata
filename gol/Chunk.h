#ifndef GAME_OF_LIFE_CHUNK_H
#define GAME_OF_LIFE_CHUNK_H

#include <unordered_map>
#include <utility>

#include "Side.h"
#include "util.h"

#define CHUNK_SIZE 20

// A "chunk" of cells which are all processed at once
class Chunk {
public:
  void tick(); // move the entire Chunk to the next generation
  void tick(Side side); // like tick(), but only update the cells whose neighbourhoods run off on the given Side
  
  // Get the value of the cell at (x, y).
  // Throw std::invalid_parameter if x >= CHUNK_SIZE, y >= CHUNK_SIZE, x < 0, or y < 0.
  bool getCell(int x, int y) const;
  
private:
  bool cells_[CHUNK_SIZE][CHUNK_SIZE];
};

// A 2D-indexed list of Chunks - a thin wrapper over std::unordered_map
// This isn't just a typedef for std::unordered_map<std::pair<int, int>, Chunk> because it'll encapsulate a Topology
// TODO have a Topology, once that's implemented
class ChunkArray {
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::iterator iterator;
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::const_iterator const_iterator;
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::size_type size_type;
  
public:
  // How many Chunks are stored?
  size_type size();
  
  // Get the Chunk at (x, y), creating one if not present
  Chunk& get(int x, int y);
  Chunk& operator[](std::pair<int, int> xy);
  
  // Does this ChunkArray contain a Chunk at (x, y)?
  bool contains(int x, int y) const;
  
  // Erase the Chunk at (x, y) if present, returning whether a Chunk was erased
  bool erase(int x, int y);
  
  // Iterators
  iterator begin() noexcept;
  iterator end() noexcept;
  const_iterator begin() const noexcept;
  const_iterator end() const noexcept;
  
private:
  std::unordered_map<std::pair<int, int>, Chunk, pair_hash> map_;
};

#endif //GAME_OF_LIFE_CHUNK_H
