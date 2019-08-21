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
// To update a cell, first call Chunk::generate(Ruleset&, Neighbourhood&) (or call the overloaded Side& version with
// as many sides as needed), then call Chunk::update() to process the next generation.
class Chunk {
public:
  // Initialize the Chunk with the specified coordinates
  Chunk(int x, int y) noexcept;
  
  // Evaluate all the cells in the Chunk using the specified ruleset and prime the chunk to update.
  // If side and affectingDistance are specified, evaluate only the cells within affectingDistance of side.
  // (Note: this is overloaded instead of using default arguments because they're not allowed on virtual methods.)
  virtual void generate(const Ruleset& ruleset, Neighbourhood& neighbourhood, const Side& side,
      unsigned int affectingDistance);
  virtual void generate(const Ruleset& ruleset, Neighbourhood& neighbourhood);
  
  // Move to the next generation which was generated by Chunk::generate. You must call Chunk::generate first for
  // this to have any effect. Update what will be returned by Chunk::getCell(x, y).
  virtual void update();
  
  // Get the value of the cell at (x, y).
  // Throw std::invalid_argument if x >= CHUNK_SIZE, y >= CHUNK_SIZE, x < 0, or y < 0.
  virtual bool getCell(int x, int y) const;
  
  virtual bool isEmpty() const noexcept; // Is the whole Chunk empty?
  virtual bool isNextGenEmpty() const noexcept; // Is the next generation currently empty?
  
private:
  // Update the cell at (x, y), assuming the neighbourhood is centred at (x, y), using the ruleset.
  void updateNewCell(const Ruleset& ruleset, Neighbourhood& neighbourhood, int x, int y);
  
  // Scan a single line left or right with reference to the optionally given side. Modifies x.
  void scanLine(const Ruleset& ruleset, Neighbourhood& neighbourhood, int& x, int y, const Side& side = Side::BOTTOM);
  
  int x_, y_; // coordinates of this Chunk
  bool cells_[CHUNK_SIZE][CHUNK_SIZE] = {}; // the cells in the Chunk; index like cells_[x][y]
  bool newCells_[CHUNK_SIZE][CHUNK_SIZE] = {}; // the cells of the next generation
  bool empty_ = true; // is the Chunk completely empty?
  bool nextGenEmpty_ = true; // is the next generation of the Chunk currently completely empty?
};

// A 2D-indexed list of Chunks - a thin wrapper over std::unordered_map.
// This isn't just a typedef for std::unordered_map<std::pair<int, int>, Chunk> because it encapsulates a Topology.
// If the Topology deems a coordinate invalid, a blank Chunk is returned.
// The Topology may modify coordinates as it wishes.
// All hail the great and mighty Topology.
class ChunkArray {
public:
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::iterator iterator;
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::const_iterator const_iterator;
  typedef std::unordered_map<std::pair<int, int>, Chunk, pair_hash>::size_type size_type;
  
  // Initialize this ChunkArray with the specified Topology
  explicit ChunkArray(Topology* topology);
  
  // How many Chunks are stored?
  size_type size();
  
  // Get the Chunk at (x, y), creating one if not present
  Chunk& get(int x, int y);
  
  // Does this ChunkArray contain a Chunk at (x, y)?
  bool contains(int x, int y) const;
  
  // Does this ChunkArray contain a non-empty Chunk at (x, y)?
  bool hasNonEmpty(int x, int y);
  
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
    void generate(const Ruleset&, Neighbourhood&, const Side&, unsigned int) override {}
    void generate(const Ruleset&, Neighbourhood&) override {}
    void update() override {}
    bool getCell(int, int) const override { return false; }
    bool isEmpty() const noexcept override { return true; }
    bool isNextGenEmpty() const noexcept override { return true; }
  } static EMPTY;
  
  std::unordered_map<std::pair<int, int>, Chunk, pair_hash> map_;
  std::unique_ptr<Topology> topology_;
};

#endif //GAME_OF_LIFE_CHUNK_H
