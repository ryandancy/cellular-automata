#include <stdexcept>
#include <utility>

#include "Chunk.h"

// Chunk

Chunk::Chunk(int x, int y) noexcept : x_(x), y_(y) {}

void Chunk::updateCell(const Ruleset& ruleset, Neighbourhood& neighbourhood, int x, int y) {
  unsigned int liveCount = neighbourhood.getLiveCount();
  if (cells_[x][y]) {
    cells_[x][y] = ruleset.survivesWith(liveCount);
  } else {
    cells_[x][y] = ruleset.isBornWith(liveCount);
  }
  if (empty_ && cells_[x][y]) {
    empty_ = false;
  }
}

void Chunk::scanLine(const Ruleset& ruleset, Neighbourhood& neighbourhood, int& x, int y, const Side& side) {
  if (x == 0) {
    do {
      x = neighbourhood.moveToSide(side.right());
      updateCell(ruleset, neighbourhood, x, y);
    } while (x < CHUNK_SIZE - 1);
  } else {
    do {
      x = neighbourhood.moveToSide(side.left());
      updateCell(ruleset, neighbourhood, x, y);
    } while (x > 0);
  }
}

void Chunk::tick(const Ruleset& ruleset, Neighbourhood& neighbourhood, const Side& side,
    unsigned int affectingDistance) {
  // Scan-line: we start at (0, 0) or equivalent, move right to (CHUNK_SIZE-1, 0), move down 1, move left to (0, 1), etc
  // We treat it like the side is Side::BOTTOM always and transform on function calls
  int x = 0, y = CHUNK_SIZE - affectingDistance;
  neighbourhood.moveTo(x_, y_, x, y);
  
  if (affectingDistance == CHUNK_SIZE) {
    empty_ = true; // until proven otherwise
  }
  
  scanLine(ruleset, neighbourhood, x, y, side);
  while (y < CHUNK_SIZE - 1) {
    y = neighbourhood.moveToSide(side);
    updateCell(ruleset, neighbourhood, x, y);
    scanLine(ruleset, neighbourhood, x, y, side);
  }
}

void Chunk::tick(const Ruleset& ruleset, Neighbourhood& neighbourhood) {
  // Overload, not default, because "default arguments prohibited on virtual or override methods"
  tick(ruleset, neighbourhood, Side::CONST_BOTTOM, CHUNK_SIZE); // CONST_BOTTOM to not construct another Side
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

// ChunkArray

decltype(ChunkArray::EMPTY) ChunkArray::EMPTY(0, 0);

ChunkArray::ChunkArray(std::shared_ptr<Topology>& topology) : topology_(topology) {} // copy reference for ourselves

ChunkArray::size_type ChunkArray::size() {
  return map_.size();
}

bool ChunkArray::contains(int x, int y) const {
  bool ok = topology_->transform(x, y);
  return ok && map_.count({x, y}) == 1;
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
