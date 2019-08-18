#ifndef GAME_OF_LIFE_CHUNK_H
#define GAME_OF_LIFE_CHUNK_H

#include <memory>
#include <unordered_map>
#include <utility>

#include "Neighbourhood.h"
#include "Ruleset.h"
#include "Side.h"
#include "Topology.h"
#include "util.h"

#define CHUNK_SIZE 20

// A "chunk" of cells which are all processed at once.
class Chunk {
public:
  // Initialize the Chunk with the specified coordinates
  Chunk(int x, int y) noexcept;
  
  // Move the entire Chunk to the next generation using the given Neighbourhood, which will be moved around.
  // Evaluate all the cells in the Chunk using the Ruleset and update what Chunk::getCell(x, y) will return.
  virtual void tick(Ruleset& ruleset, Neighbourhood& neighbourhood);
  
  // Move only the cells within affectingDistance of the given side to the next generation using the given
  // Neighbourhood and Ruleset. This should be used when the chunk is empty, but adjacent to a non-empty chunk.
  virtual void tickSide(Ruleset& ruleset, Neighbourhood& neighbourhood, Side side, int affectingDistance);
  
  // Get the value of the cell at (x, y).
  // Throw std::invalid_argument if x >= CHUNK_SIZE, y >= CHUNK_SIZE, x < 0, or y < 0.
  virtual bool getCell(int x, int y) const;
  
  // Is the whole chunk empty?
  virtual bool isEmpty() const noexcept;
  
private:
  // Update the cell at (x, y), assuming the neighbourhood is centred at (x, y), using the ruleset.
  void updateCell(Ruleset& ruleset, Neighbourhood& neighbourhood, int x, int y);
  
  // Scan a single line left or right. Modifies x.
  void scanLine(Ruleset& ruleset, Neighbourhood& neighbourhood, int& x, int y);
  
  int x_, y_; // coordinates of this Chunk
  bool cells_[CHUNK_SIZE][CHUNK_SIZE] = {}; // the cells in the Chunk; index like cells_[x][y]
  bool empty_ = true; // is the Chunk completely empty?
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
    using Chunk::Chunk;
    void tick(Ruleset&, Neighbourhood&) override {}
    void tickSide(Ruleset&, Neighbourhood&, Side, int) override {}
    bool getCell(int x, int y) const override { return false; }
    bool isEmpty() const noexcept override { return true; }
  } static EMPTY;
  
  std::unordered_map<std::pair<int, int>, Chunk, pair_hash> map_;
  std::shared_ptr<Topology> topology_;
};

#endif //GAME_OF_LIFE_CHUNK_H
