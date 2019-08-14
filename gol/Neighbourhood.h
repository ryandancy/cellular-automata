#ifndef GAME_OF_LIFE_NEIGHBOURHOOD_H
#define GAME_OF_LIFE_NEIGHBOURHOOD_H

#include "Chunk.h"

// This is an abstract base class representing the neighbourhood of a cell. Neighbourhoods can be moved around a
// ChunkArray in order to save time. One Neighbourhood is created for each generation. Note that after creating a
// Neighbourhood, you must call Neighbourhood::moveTo in order to initialize it before using the other methods.
class Neighbourhood {
public:
  // Move this Neighbourhood to (x, y) in the chunk at coordinates (chunkX, chunkY) in the ChunkArray.
  // Throw std::invalid_argument if x > CHUNK_SIZE, y > CHUNK_SIZE, or there is no Chunk at (chunkX, chunkY).
  void moveTo(unsigned int x, unsigned int y, int chunkX, int chunkY);
  
  unsigned int getX() const noexcept; // Get the x coordinate of the Neighbourhood in its chunk. Return <= CHUNK_SIZE.
  unsigned int getY() const noexcept; // Get the y coordinate of the Neighbourhood in its chunk. Return <= CHUNK_SIZE.
  
  // Move the Neighbourhood one cell to the right in its chunk, updating the live count. Return the new x coordinate.
  // Throw std::range_error if the x coordinate is already CHUNK_SIZE and we try to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  virtual unsigned int moveRight() = 0;
  
  // Move the Neighbourhood one cell to the left in its chunk, updating the live count. Return the new x coordinate.
  // Throw std::range_error if the x coordinate is already 0 and we try to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  virtual unsigned int moveLeft() = 0;
  
  // Move the Neighbourhood one cell down in its chunk, updating the live count. Return the new y coordinate.
  // Throw std::range_error if the y coordinate is already CHUNK_SIZE and we try to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  virtual unsigned int moveDown() = 0;
  
  // virtual unsigned int moveUp() = 0; // not necessary for right-to-left scanning
  
  // Get the live count of the Neighbourhood, i.e. the number of live cells in the Neighbourhood.
  unsigned int getLiveCount() const noexcept;
  
protected:
  // Initialize the Neighbourhood with the given ChunkArray. This Neighbourhood must be destroyed before the
  // ChunkArray passed here.
  explicit Neighbourhood(const ChunkArray& chunkArray);
  
  // Check the Neighbourhood::ready_ flag and throw std::logic_error if it is not true.
  void verifyReady() const;
  
  // Take stock of the Neighbourhood's surroundings from scratch. Recreate the live count.
  // This is called from moveTo(unsigned, unsigned, int, int) to update from scratch.
  virtual void reinitialize() = 0;
  
  unsigned int x_;
  unsigned int y_;
  int chunkX_;
  int chunkY_;
  unsigned int liveCount_;
  const ChunkArray& chunkArray_;
  
  bool ready_; // are we in a valid state to do anything (i.e. has moveTo() been called)? If not, error.
};

// This abstract class represents the *type* of a Neighbourhood (i.e. von Neumann, Moore, or something else).
// It functions as a factory class with NeighbourhoodType::makeNeighbourhood(ChunkArray&), but also provides
// type-level information about the Neighbourhood type, such as the total number of cells it has and the maximum
// distance away a cell which could affect the centre cell could be. Objects of this type are immutable.
class NeighbourhoodType {
public:
  // Create a Neighbourhood of this type. Caller is responsible for managing the pointer. Note that the returned
  // Neighbourhood must be destroyed before the ChunkArray passed here - otherwise, the Neighbourhood will error.
  virtual Neighbourhood* makeNeighbourhood(ChunkArray& chunkArray) const = 0;
  
  // Get the total number of cells in Neighbourhoods of this type.
  unsigned int getNumCells() const;
  
  // Get the maximum distance at which the centre cell in the Neighbourhood could possibly be affected by
  // a cell at this distance. Used to determine how far into empty chunks to evaluate - a cell in an empty chunk
  // is evaluated if both its x and y coordinates are no more than this far from a non-empty chunk.
  unsigned int getAffectingDistance() const;
  
protected:
  explicit NeighbourhoodType(unsigned int numCells, unsigned int affectingDistance);
  
private:
  const unsigned int numCells_;
  const unsigned int affectingDistance_;
};

#endif //GAME_OF_LIFE_NEIGHBOURHOOD_H
