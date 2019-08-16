#ifndef GAME_OF_LIFE_CHUNK_H
#define GAME_OF_LIFE_CHUNK_H

#include <memory>
#include <unordered_map>
#include <utility>

#include "Side.h"
#include "Topology.h"
#include "util.h"

#define CHUNK_SIZE 20

// A "chunk" of cells which are all processed at once
class Chunk {
public:
  virtual void tick(); // move the entire Chunk to the next generation
  virtual void tick(Side side); // like tick(), but only update the cells whose neighbourhoods run off on the given Side
  
  // Get the value of the cell at (x, y).
  // Throw std::invalid_argument if x >= CHUNK_SIZE, y >= CHUNK_SIZE, x < 0, or y < 0.
  virtual bool getCell(int x, int y) const;
  
private:
  bool cells_[CHUNK_SIZE][CHUNK_SIZE];
};

// A 2D-indexed list of Chunks - a thin wrapper over std::unordered_map.
// This isn't just a typedef for std::unordered_map<std::pair<int, int>, Chunk> because it encapsulates a Topology.
// If the Topology deems a coordinate invalid, a blank Chunk is returned.
// The Topology may modify coordinates as it wishes.
// All hail the great and mighty Topology.
class ChunkArray {
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::iterator iterator;
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::const_iterator const_iterator;
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::size_type size_type;
  
public:
  // Initialize this ChunkArray with the specified Topology
  explicit ChunkArray(std::shared_ptr<Topology>& topology);
  
  // How many Chunks are stored?
  size_type size();
  
  // Get the Chunk at (x, y), creating one if not present
  Chunk& get(int x, int y);
  
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
  // Empty Chunk for use when the Topology specifies a chunk is to be treated as empty
  struct : public Chunk {
    void tick() override {}
    void tick(Side side) override {}
    bool getCell(int x, int y) const override { return false; }
  } static EMPTY;
  
  std::unordered_map<std::pair<int, int>, Chunk, pair_hash> map_;
  std::shared_ptr<Topology> topology_;
};

#endif //GAME_OF_LIFE_CHUNK_H
