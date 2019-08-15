#include <stdexcept>
#include "Chunk.h"

// Chunk

void Chunk::tick() {
  // TODO
}

void Chunk::tick(Side side) {
  // TODO
}

bool Chunk::getCell(int x, int y) const {
  if (x < 0 || y < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE) {
    throw std::logic_error("Cannot get cell with x or y greater than or equal to CHUNK_SIZE or less than 0");
  }
  return cells_[x][y];
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
