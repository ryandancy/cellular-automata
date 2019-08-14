#include <stdexcept>
#include "Neighbourhood.h"

// Neighbourhood

void Neighbourhood::moveTo(unsigned int x, unsigned int y, int chunkX, int chunkY) {
  x_ = x;
  y_ = y;
  chunkX_ = chunkX;
  chunkY_ = chunkY;
  ready_ = true;
  reinitialize();
}

unsigned int Neighbourhood::getX() const noexcept {
  return x_;
}

unsigned int Neighbourhood::getY() const noexcept {
  return y_;
}

unsigned int Neighbourhood::getLiveCount() const noexcept {
  return liveCount_;
}

// Initialize the chunk array, but set ready_ to false
Neighbourhood::Neighbourhood(const ChunkArray& chunkArray)
  : chunkArray_(chunkArray), x_(0), y_(0), chunkX_(0), chunkY_(0), liveCount_(0), ready_(false) {}

void Neighbourhood::verifyReady() const {
  if (!ready_) {
    throw std::logic_error("Neighbourhood is not ready");
  }
}

// NeighbourhoodType

NeighbourhoodType::NeighbourhoodType(unsigned int numCells, unsigned int affectingDistance)
  : numCells_(numCells), affectingDistance_(affectingDistance) {}

unsigned int NeighbourhoodType::getNumCells() const {
  return numCells_;
}

unsigned int NeighbourhoodType::getAffectingDistance() const {
  return affectingDistance_;
}
