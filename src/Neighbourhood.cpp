#include <stdexcept>
#include "Neighbourhood.h"

// I sincerely apologize, future self, for all the DRY violations.

// Neighbourhood

void Neighbourhood::moveTo(int x, int y, int chunkX, int chunkY) {
  if (x < 0 || y < 0 || x >= CHUNK_SIZE || y >= CHUNK_SIZE) {
    throw std::invalid_argument("Cannot move to x or y less than 0 or greater than or equal to CHUNK_SIZE");
  }
  if (!chunkArray_.contains(chunkX, chunkY)) {
    throw std::invalid_argument("Cannot move to chunk that does not exist");
  }
  x_ = x;
  y_ = y;
  chunkX_ = chunkX;
  chunkY_ = chunkY;
  ready_ = true;
  reinitialize();
}

int Neighbourhood::getX() const noexcept {
  return x_;
}

int Neighbourhood::getY() const noexcept {
  return y_;
}

unsigned int Neighbourhood::getLiveCount() const noexcept {
  return liveCount_;
}

int Neighbourhood::moveRight() {
  // Check that we can move right from here
  verifyReady();
  if (x_ == CHUNK_SIZE - 1) {
    throw std::range_error("Neighbourhood cannot move right: already at maximum for chunk (CHUNK_SIZE - 1)");
  }
  
  translateRight();
  
  // Add the old centre cell and subtract the new centre cell
  if (getCell(0, 0)) liveCount_++;
  if (getCell(1, 0)) liveCount_--;
  
  return ++x_;
}

int Neighbourhood::moveLeft() {
  // Check that we can move left from here
  verifyReady();
  if (x_ == 0) {
    throw std::range_error("Neighbourhood cannot move left: already at minimum for chunk (0)");
  }
  
  translateLeft();
  
  // Add the old centre cell and subtract the new centre cell
  if (getCell(0, 0)) liveCount_++;
  if (getCell(-1, 0)) liveCount_--;
  
  return --x_;
}

int Neighbourhood::moveDown() {
  // Check that we can move down from here
  verifyReady();
  if (y_ == CHUNK_SIZE - 1) {
    throw std::range_error("Neighbourhood cannot move down: already at maximum for chunk (CHUNK_SIZE - 1)");
  }
  
  translateDown();
  
  // Add the old centre cell and subtract the new centre cell
  if (getCell(0, 0)) liveCount_++;
  if (getCell(0, 1)) liveCount_--;
  
  return ++y_;
}

// Initialize the chunk array, but set ready_ to false
Neighbourhood::Neighbourhood(ChunkArray& chunkArray)
  : chunkArray_(chunkArray), x_(0), y_(0), chunkX_(0), chunkY_(0), liveCount_(0), ready_(false) {}

void Neighbourhood::verifyReady() const {
  if (!ready_) {
    throw std::logic_error("Neighbourhood is not ready");
  }
}

bool Neighbourhood::getCell(int dx, int dy) const {
  verifyReady();
  
  // To handle moving out of the current chunk, we manipulate these variables such that
  // the coordinates of the cell to get from are (nx, ny) in chunk (ncx, ncy).
  int nx = x_ + dx;
  int ny = y_ + dy;
  int ncx = chunkX_;
  int ncy = chunkY_;
  
  if (nx < 0) {
    // moving out of the chunk to the left
    int numMoving = -((nx+1) / CHUNK_SIZE) + 1;
    ncx -= numMoving;
    nx %= CHUNK_SIZE;
    nx += CHUNK_SIZE; // % in C++ makes negatives remain negative
  } else if (nx >= CHUNK_SIZE) {
    // moving out of the chunk to the right
    ncx += nx / CHUNK_SIZE;
    nx %= CHUNK_SIZE;
  }
  
  if (ny < 0) {
    // moving out of the chunk to the top
    int numMoving = -((ny+1) / CHUNK_SIZE) + 1;
    ncy -= numMoving;
    ny %= CHUNK_SIZE;
    ny += CHUNK_SIZE;
  } else if (ny >= CHUNK_SIZE) {
    // moving out of the chunk to the bottom
    ncy += ny / CHUNK_SIZE;
    ny %= CHUNK_SIZE;
  }
  
  return chunkArray_.get(ncx, ncy).getCell(nx, ny);
}

// NeighbourhoodType

NeighbourhoodType::NeighbourhoodType(unsigned int numCells, int affectingDistance)
    : numCells_(numCells), affectingDistance_(affectingDistance) {}

unsigned int NeighbourhoodType::getNumCells() const {
  return numCells_;
}

int NeighbourhoodType::getAffectingDistance() const {
  return affectingDistance_;
}

// Helper function for error checking
namespace { // local to this file
  void checkRadius(int radius) {
    if (radius <= 0) {
      throw std::invalid_argument("Radius of neighbourhood must be positive");
    }
  }
}

// MooreNeighbourhoodType

MooreNeighbourhoodType::MooreNeighbourhoodType(int radius)
    : NeighbourhoodType(radius*radius - 1, radius), radius_(radius) {
  checkRadius(radius_);
}

Neighbourhood* MooreNeighbourhoodType::makeNeighbourhood(ChunkArray& chunkArray) const {
  return new MooreNeighbourhood(chunkArray, radius_);
}

// MooreNeighbourhood

MooreNeighbourhood::MooreNeighbourhood(ChunkArray& chunkArray, int radius)
    : Neighbourhood(chunkArray), radius_(radius) {
  checkRadius(radius_);
}

void MooreNeighbourhood::reinitialize() {
  // Go through the whole square one-by-one
  liveCount_ = 0;
  for (int dx = -radius_; dx <= radius_; dx++) {
    for (int dy = -radius_; dy <= radius_; dy++) {
      if (dx == 0 && dy == 0) continue; // don't include the centre cell
      if (getCell(dx, dy)) {
        liveCount_++;
      }
    }
  }
}

void MooreNeighbourhood::translateRight() {
  // Subtract the left column and add the new right column
  for (int dy = -radius_; dy <= radius_; dy++) {
    if (getCell(-radius_, dy)) liveCount_--;
    if (getCell(radius_ + 1, dy)) liveCount_++;
  }
}

void MooreNeighbourhood::translateLeft() {
  // Subtract the right column and add the new left column
  for (int dy = -radius_; dy <= radius_; dy++) {
    if (getCell(radius_, dy)) liveCount_--;
    if (getCell(-radius_ - 1, dy)) liveCount_++;
  }
}

void MooreNeighbourhood::translateDown() {
  // Subtract the top row and add the new bottom row
  for (int dx = -radius_; dx <= radius_; dx++) {
    if (getCell(dx, -radius_)) liveCount_--;
    if (getCell(dx, radius_ + 1)) liveCount_++;
  }
}

// VonNeumannNeighbourhoodType

// Note: the number of cells in a von Neumann neighbourhood of radius r is 2r(r+1)
VonNeumannNeighbourhoodType::VonNeumannNeighbourhoodType(int radius)
    : NeighbourhoodType(2*radius*(radius+1), radius), radius_(radius) {
  checkRadius(radius_);
}

Neighbourhood* VonNeumannNeighbourhoodType::makeNeighbourhood(ChunkArray& chunkArray) const {
  return new VonNeumannNeighbourhood(chunkArray, radius_);
}

// VonNeumannNeighbourhood

VonNeumannNeighbourhood::VonNeumannNeighbourhood(ChunkArray& chunkArray, int radius)
    : Neighbourhood(chunkArray), radius_(radius) {
  checkRadius(radius_);
}

void VonNeumannNeighbourhood::reinitialize() {
  liveCount_ = 0;
  for (int dy = -radius_; dy <= radius_; dy++) {
    for (int dx = -getSideDist(dy); dx <= getSideDist(dy); dx++) {
      if (dx == 0 && dy == 0) continue; // skip centre cell
      if (getCell(dx, dy)) {
        liveCount_++;
      }
    }
  }
}

void VonNeumannNeighbourhood::translateRight() {
  // Subtract the leftmost part and add the new rightmost part
  for (int dy = -radius_; dy <= radius_; dy++) {
    if (getCell(-getSideDist(dy), dy)) liveCount_--;
    if (getCell(getSideDist(dy) + 1, dy)) liveCount_++;
  }
}

void VonNeumannNeighbourhood::translateLeft() {
  // Subtract the rightmost part and add the new leftmost part
  for (int dy = -radius_; dy <= radius_; dy++) {
    if (getCell(getSideDist(dy), dy)) liveCount_--;
    if (getCell(-getSideDist(dy) - 1, dy)) liveCount_++;
  }
}

void VonNeumannNeighbourhood::translateDown() {
  // Subtract the topmost part and add the new bottommost part
  for (int dx = -radius_; dx <= radius_; dx++) {
    if (getCell(dx, -getSideDist(dx))) liveCount_--;
    if (getCell(dx, getSideDist(dx) + 1)) liveCount_++;
  }
}

inline int VonNeumannNeighbourhood::getSideDist(int a) {
  return radius_ - abs(a);
}
