#include "Chunk.h"

// Chunk

void Chunk::tick() {
  // TODO
}

void Chunk::tick(Side side) {
  // TODO
}

// ChunkArray

ChunkArray::size_type ChunkArray::size() {
  return map_.size();
}

bool ChunkArray::contains(int x, int y) const {
  return map_.count({x, y}) == 1;
}

Chunk& ChunkArray::get(int x, int y) {
  return map_[{x, y}];
}

Chunk& ChunkArray::operator[](std::pair<int, int> xy) {
  return map_[xy];
}

bool ChunkArray::erase(int x, int y) {
  return map_.erase({x, y}) > 0;
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
