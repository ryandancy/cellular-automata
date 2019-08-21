#include <stdexcept>
#include <utility>

#include "Chunk.h"

// Chunk

Chunk::Chunk(int x, int y) noexcept : x_(x), y_(y) {}

void Chunk::updateNewCell(const Ruleset& ruleset, Neighbourhood& neighbourhood, int x, int y) {
  unsigned int liveCount = neighbourhood.getLiveCount();
  if (cells_[x][y]) {
    newCells_[x][y] = ruleset.survivesWith(liveCount);
  } else {
    newCells_[x][y] = ruleset.isBornWith(liveCount);
  }
  if (nextGenEmpty_ && cells_[x][y]) {
    nextGenEmpty_ = false;
  }
}

void Chunk::scanLine(const Ruleset& ruleset, Neighbourhood& neighbourhood, int& x, int y, const Side& side) {
  if (x == 0) {
    do {
      x = neighbourhood.moveToSide(side.right());
      updateNewCell(ruleset, neighbourhood, x, y);
    } while (x < CHUNK_SIZE - 1);
  } else {
    do {
      x = neighbourhood.moveToSide(side.left());
      updateNewCell(ruleset, neighbourhood, x, y);
    } while (x > 0);
  }
}

void Chunk::generate(const Ruleset& ruleset, Neighbourhood& neighbourhood, const Side& side,
    unsigned int affectingDistance) {
  // Scan-line: we start at (0, 0) or equivalent, move right to (CHUNK_SIZE-1, 0), move down 1, move left to (0, 1), etc
  // We treat it like the side is Side::BOTTOM always and transform on function calls
  int x = 0, y = CHUNK_SIZE - affectingDistance;
  neighbourhood.moveTo(x_, y_, x, y);
  
  scanLine(ruleset, neighbourhood, x, y, side);
  while (y < CHUNK_SIZE - 1) {
    y = neighbourhood.moveToSide(side);
    updateNewCell(ruleset, neighbourhood, x, y);
    scanLine(ruleset, neighbourhood, x, y, side);
  }
}

void Chunk::generate(const Ruleset& ruleset, Neighbourhood& neighbourhood) {
  // Overload, not default, because "default arguments prohibited on virtual or override methods"
  generate(ruleset, neighbourhood, Side::CONST_BOTTOM, CHUNK_SIZE); // CONST_BOTTOM to not construct another Side
}

void Chunk::update() {
  if (empty_ && nextGenEmpty_) {
    // no point, we won't update anything
    return;
  }
  
  // this is probably performance critical, so memcpy/memset it is
  memcpy(&cells_, &newCells_, sizeof(cells_)); // FIXME if there's a segfault, it's probably here
  memset(&newCells_, 0, sizeof(newCells_)); // the next generation starts at 0
  
  empty_ = nextGenEmpty_;
  nextGenEmpty_ = true; // until proven otherwise
}

bool Chunk::getCell(int x, int y) const {
  if (x < 0 || y < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE) {
    throw std::invalid_argument("Cannot get cell with x or y greater than or equal to CHUNK_SIZE or less than 0");
  }
  return cells_[x][y];
}

bool Chunk::isEmpty() const noexcept {
  return empty_;
}

bool Chunk::isNextGenEmpty() const noexcept {
  return nextGenEmpty_;
}

// ChunkArray

decltype(ChunkArray::EMPTY) ChunkArray::EMPTY(0, 0);

ChunkArray::ChunkArray(Topology* topology) : topology_(topology) {} // adopt the topology for ourselves

ChunkArray::size_type ChunkArray::size() {
  return map_.size();
}

Chunk& ChunkArray::get(int x, int y) {
  bool ok = topology_->transform(x, y);
  if (!ok) {
    return ChunkArray::EMPTY;
  }
  
  // Create a Chunk if it isn't there - we need to call Chunk's constructor ourselves
  std::pair<int, int> xy = {x, y};
  if (!map_.count(xy)) {
    map_.emplace(xy, Chunk(x, y));
  }
  return map_.at(xy);
}

bool ChunkArray::contains(int x, int y) const {
  bool ok = topology_->transform(x, y);
  return ok && map_.count({x, y}) == 1;
}

bool ChunkArray::hasNonEmpty(int x, int y) {
  return contains(x, y) && !get(x, y).isEmpty();
}

bool ChunkArray::erase(int x, int y) {
  bool ok = topology_->transform(x, y);
  if (ok) {
    return map_.erase({x, y}) > 0;
  } else {
    return false;
  }
}

ChunkArray::iterator ChunkArray::begin() noexcept {
  return map_.begin();
}

ChunkArray::iterator ChunkArray::end() noexcept {
  return map_.end();
}

ChunkArray::const_iterator ChunkArray::begin() const noexcept {
  return map_.begin();
}

ChunkArray::const_iterator ChunkArray::end() const noexcept {
  return map_.end();
}
