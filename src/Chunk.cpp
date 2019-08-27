#include <stdexcept>
#include <utility>

#include "Chunk.h"

// Chunk

Chunk::Chunk(int x, int y) noexcept : chunkX(x), chunkY(y) {}

void Chunk::checkInBounds(int x, int y) {
  if (x < 0 || y < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE) {
    throw std::invalid_argument("Cannot set or get cell with x or y not in [0, CHUNK_SIZE).");
  }
}

void Chunk::updateNewCell(const Ruleset& ruleset, Neighbourhood& neighbourhood, int x, int y, const Side& side) {
  side.transform(x, y, CHUNK_SIZE, CHUNK_SIZE);
  checkInBounds(x, y);
  unsigned int liveCount = neighbourhood.getLiveCount();
  if (cells_[x][y]) {
    newCells_[x][y] = ruleset.survivesWith(liveCount);
  } else {
    newCells_[x][y] = ruleset.isBornWith(liveCount);
  }
  if (newCells_[x][y]) {
    nextLiveCellCount_++;
  }
}

void Chunk::scanLine(const Ruleset& ruleset, Neighbourhood& neighbourhood, int& x, int y, const Side& side) {
  if (x == 0) {
    do {
      neighbourhood.moveToSide(side.right());
      x++;
      updateNewCell(ruleset, neighbourhood, x, y, side);
    } while (x < CHUNK_SIZE - 1);
  } else {
    do {
      neighbourhood.moveToSide(side.left());
      x--;
      updateNewCell(ruleset, neighbourhood, x, y, side);
    } while (x > 0);
  }
}

void Chunk::generate(const Ruleset& ruleset, Neighbourhood& neighbourhood, const Side& side,
    unsigned int affectingDistance) {
  // Scan-line: we start at (0, 0) or equivalent, move right to (CHUNK_SIZE-1, 0), move down 1, move left to (0, 1), etc
  // We treat it like the side is Side::BOTTOM always and transform on function calls
  int x = 0, y = CHUNK_SIZE - affectingDistance;
  int relStartX = x, relStartY = y;
  side.transform(relStartX, relStartY, CHUNK_SIZE, CHUNK_SIZE);
  neighbourhood.moveTo(chunkX, chunkY, relStartX, relStartY);
  updateNewCell(ruleset, neighbourhood, x, y, side);
  
  scanLine(ruleset, neighbourhood, x, y, side);
  while (y < CHUNK_SIZE - 1) {
    neighbourhood.moveToSide(side);
    y++;
    updateNewCell(ruleset, neighbourhood, x, y, side);
    scanLine(ruleset, neighbourhood, x, y, side);
  }
}

void Chunk::generate(const Ruleset& ruleset, Neighbourhood& neighbourhood) {
  // Overload, not default, because "default arguments prohibited on virtual or override methods"
  generate(ruleset, neighbourhood, Side::CONST_BOTTOM, CHUNK_SIZE); // CONST_BOTTOM to not construct another Side
}

void Chunk::update() {
  if (isEmpty() && isNextGenEmpty()) {
    // no point, we won't update anything
    return;
  }
  
  // this is probably performance critical, so memcpy/memset it is
  memcpy(cells_, newCells_, CHUNK_SIZE*CHUNK_SIZE*sizeof(cells_[0][0]));
  memset(newCells_, 0, CHUNK_SIZE*CHUNK_SIZE*sizeof(newCells_[0][0])); // the next generation starts at 0
  
  liveCellCount_ = nextLiveCellCount_;
  nextLiveCellCount_ = 0; // until proven otherwise
  
  emit chunkChanged(cells_);
}

bool Chunk::getCell(int x, int y) const {
  checkInBounds(x, y);
  return cells_[x][y];
}

void Chunk::setCell(int x, int y, bool value) {
  checkInBounds(x, y);
  if (cells_[x][y] != value) { // prevent emitting signals when nothing changes
    cells_[x][y] = value;
    if (value) {
      liveCellCount_++;
    } else {
      liveCellCount_--;
    }
    emit chunkChanged(cells_);
  }
}

bool Chunk::isEmpty() const noexcept {
  return liveCellCount_ == 0;
}

bool Chunk::isNextGenEmpty() const noexcept {
  return nextLiveCellCount_ == 0;
}

// ChunkArray

decltype(ChunkArray::EMPTY) ChunkArray::EMPTY(0, 0);

ChunkArray::ChunkArray(Topology* topology) : topology_(topology) {} // adopt the topology for ourselves

ChunkArray::~ChunkArray() {
  // delete all the Chunks
  for (const std::pair<const std::pair<int, int>, Chunk*>& element : map_) {
    emit chunkRemoved(element.first.first, element.first.second);
    delete element.second;
  }
}

Topology& ChunkArray::topology() const noexcept {
  return *topology_;
}

ChunkArray::size_type ChunkArray::size() {
  return map_.size();
}

Chunk& ChunkArray::at(int x, int y) const {
  bool ok = topology_->transform(x, y);
  if (!ok) {
    return ChunkArray::EMPTY;
  }
  return *map_.at({x, y}); // throws std::out_of_range if not present
}

bool ChunkArray::contains(int x, int y) const {
  bool ok = topology_->transform(x, y);
  return ok && map_.count({x, y}) == 1;
}

bool ChunkArray::hasNonEmpty(int x, int y) {
  return contains(x, y) && !at(x, y).isEmpty();
}

bool ChunkArray::insertOrNoop(int x, int y) {
  bool ok = topology_->transform(x, y);
  if (!ok) {
    return false;
  }
  
  std::pair<int, int> xy(x, y);
  if (map_.count(xy)) {
    return false;
  }
  map_.emplace(xy, new Chunk(x, y));
  emit chunkAdded(x, y);
  return true;
}

void ChunkArray::queueForInsertion(int x, int y) {
  if (!ignoreQueueInsertion_) {
    coordinateQueue_.emplace(std::make_pair(x, y));
  }
}

void ChunkArray::insertAllInQueue() {
  for (const std::pair<int, int>& xy : coordinateQueue_) {
    insertOrNoop(xy.first, xy.second);
  }
}

void ChunkArray::clearQueue() {
  coordinateQueue_.clear();
}

void ChunkArray::setIgnoringQueueInsertions(bool ignore) {
  ignoreQueueInsertion_ = ignore;
}

ChunkArray::queue_iterator ChunkArray::queueBegin() noexcept {
  return coordinateQueue_.begin();
}

ChunkArray::queue_iterator ChunkArray::queueEnd() noexcept {
  return coordinateQueue_.end();
}

bool ChunkArray::erase(int x, int y) {
  bool ok = topology_->transform(x, y);
  if (ok) {
    // Destruct the Chunk
    std::pair<int, int> xy(x, y);
    if (map_.count(xy)) {
      delete map_.at(xy);
      map_.erase(xy);
      emit chunkRemoved(x, y);
      return true;
    }
  }
  return false;
}

void ChunkArray::clear() {
  for (auto it = map_.begin(), nextIt = it; it != map_.end(); it = nextIt) {
    ++nextIt;
    delete it->second;
    map_.erase(it->first);
    emit chunkRemoved(it->first.first, it->first.second);
  }
  clearQueue();
}

ChunkArray::iterator ChunkArray::begin() noexcept {
  return map_.begin();
}

ChunkArray::iterator ChunkArray::end() noexcept {
  return map_.end();
}
