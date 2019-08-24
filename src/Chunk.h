#ifndef GAME_OF_LIFE_CHUNK_H
#define GAME_OF_LIFE_CHUNK_H

#include <memory>
#include <unordered_set>
#include <unordered_map>
#include <utility>

#include <QObject>

#include "Neighbourhood.h"
#include "Ruleset.h"
#include "Side.h"
#include "Topology.h"
#include "util.h"

#define CHUNK_SIZE 20

// A "chunk" of cells which are all processed at once.
// To update a cell, first call Chunk::generate(Ruleset&, Neighbourhood&) (or call the overloaded Side& version with
// as many sides as needed), then call Chunk::update() to process the next generation.
class Chunk : public QObject {
  Q_OBJECT
  
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
  
  // Set the value of the cell at (x, y).
  // Throw std::invalid_argument if x >= CHUNK_SIZE, y >= CHUNK_SIZE, x < 0, or y < 0.
  virtual void setCell(int x, int y, bool value);
  
  virtual bool isEmpty() const noexcept; // Is the whole Chunk empty?
  virtual bool isNextGenEmpty() const noexcept; // Is the next generation currently empty? TODO necessary?
  
  const int chunkX, chunkY; // The coordinates of this Chunk.
  
signals:
  void chunkChanged(bool newCells[CHUNK_SIZE][CHUNK_SIZE]);

private:
  // Check that 0 <= x < CHUNK_SIZE and 0 <= y < CHUNK_SIZE, throwing std::invalid_argument otherwise.
  static void checkInBounds(int x, int y);
  
  // Update the cell at (x, y), assuming the neighbourhood is centred at (x, y), using the ruleset.
  void updateNewCell(const Ruleset& ruleset, Neighbourhood& neighbourhood, int x, int y,
      const Side& side = Side::BOTTOM);
  
  // Scan a single line left or right with reference to the optionally given side. Modifies x.
  void scanLine(const Ruleset& ruleset, Neighbourhood& neighbourhood, int& x, int y, const Side& side = Side::BOTTOM);
  
  bool cells_[CHUNK_SIZE][CHUNK_SIZE] = {}; // the cells in the Chunk; index like cells_[x][y]
  bool newCells_[CHUNK_SIZE][CHUNK_SIZE] = {}; // the cells of the next generation
  int liveCellCount_ = 0; // count the number of live cells, currently; used for determining if it's empty
  int nextLiveCellCount_ = 0; // same for the next generation
};

// A 2D-indexed list of Chunks - a thin wrapper over std::unordered_map.
// This isn't just a typedef for std::unordered_map<std::pair<int, int>, Chunk> because it encapsulates a Topology.
// If the Topology deems a coordinate invalid, a blank Chunk is returned.
// The Topology may modify coordinates as it wishes.
// All hail the great and mighty Topology.
// TODO I feel like there's something semantically wrong with returning a reference in get() and pointers via iterators
// TODO Do we really need the bool return types on insertOrNoop and erase?
class ChunkArray : public QObject {
  Q_OBJECT
  
public:
  typedef std::unordered_map<std::pair<int, int>, Chunk*, pair_hash>::iterator iterator;
  typedef std::unordered_map<std::pair<int, int>, Chunk*, pair_hash>::size_type size_type;
  
  typedef std::unordered_set<std::pair<int, int>, pair_hash>::iterator queue_iterator;
  
  // Initialize this ChunkArray with the specified Topology
  explicit ChunkArray(Topology* topology);
  
  ~ChunkArray() override; // we need to clear the map; QObject disables copy/move constructors/assignment operators
  
  // How many Chunks are stored?
  size_type size();
  
  // Get a reference to the Chunk at (x, y), throwing std::out_of_range if not present
  Chunk& at(int x, int y) const;
  
  // Does this ChunkArray contain a Chunk at (x, y)?
  bool contains(int x, int y) const;
  
  // Does this ChunkArray contain a non-empty Chunk at (x, y)?
  bool hasNonEmpty(int x, int y);
  
  // Attempt to insert an empty Chunk at (x, y). Return whether a Chunk was inserted. Does nothing otherwise.
  bool insertOrNoop(int x, int y);
  
  // Queue a chunk position (x, y) such that the next time insertAllInQueue() is called, a Chunk will be inserted at
  // (x, y). This is useful when insertion must be delayed until iterating over the current chunks is done.
  void queueForInsertion(int x, int y);
  
  // Insert all chunk positions previously queued via queueForInsertion(x, y). Do not clear the queue.
  void insertAllInQueue();
  
  // If you call setIgnoringQueueInsertions(true), until you call setIgnoringQueueInsertions(false), queueForInsertion()
  // will ignore all calls. This is useful when you know no useful chunks will be queued.
  void setIgnoringQueueInsertions(bool ignore);
  
  // Clear the queue of coordinates to insert.
  void clearQueue();
  
  // Iterators over the queue of coordinates to insert.
  queue_iterator queueBegin() noexcept;
  queue_iterator queueEnd() noexcept;
  
  // Erase the Chunk at (x, y) if present, returning whether a Chunk was erased.
  bool erase(int x, int y);
  
  // Erase all Chunks and clear the queue.
  void clear();
  
  // Iterators, iterating over pairs of coordinate pairs and pointers to corresponding Chunks
  iterator begin() noexcept;
  iterator end() noexcept;
  
signals:
  void chunkAdded(int x, int y);
  void chunkRemoved(int x, int y);
  
private:
  // Empty Chunk for use when the Topology specifies a chunk is to be treated as empty
  struct : public Chunk {
    using Chunk::Chunk;
#pragma clang diagnostic push
#pragma ide diagnostic ignored "OCUnusedGlobalDeclarationInspection"
    void generate(const Ruleset&, Neighbourhood&, const Side&, unsigned int) override {}
    void generate(const Ruleset&, Neighbourhood&) override {}
    void update() override {}
    bool getCell(int, int) const override { return false; }
    void setCell(int, int, bool) override {} // prevent emitting signals
    bool isEmpty() const noexcept override { return true; }
    bool isNextGenEmpty() const noexcept override { return true; }
#pragma clang diagnostic pop
  } static EMPTY;
  
  std::unordered_map<std::pair<int, int>, Chunk*, pair_hash> map_;
  std::unique_ptr<Topology> topology_;
  
  std::unordered_set<std::pair<int, int>, pair_hash> coordinateQueue_; // holds coordinates queued for insertion
  bool ignoreQueueInsertion_ = false;
};

#endif //GAME_OF_LIFE_CHUNK_H
