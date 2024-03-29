#ifndef GAME_OF_LIFE_NEIGHBOURHOOD_H
#define GAME_OF_LIFE_NEIGHBOURHOOD_H

#include "Side.h"

// we only have references to ChunkArray so it's safe to forward declare it without including Chunk.h
// this resolves a circular dependency chain: Neighbourhood->ChunkArray->Chunk->Neighbourhood
class ChunkArray;

// This is an abstract base class representing the neighbourhood of a cell. Neighbourhoods can be moved around a
// ChunkArray in order to save time. One Neighbourhood is created for each generation. Note that after creating a
// Neighbourhood, you must call Neighbourhood::moveTo in order to initialize it before using the other methods.
class Neighbourhood {
public:
  // Move this Neighbourhood to (x, y) in the chunk at coordinates (chunkX, chunkY) in the ChunkArray.
  // Throw std::invalid_argument if x or y are not in [0, CHUNK_SIZE), or if there is no Chunk at (chunkX, chunkY).
  void moveTo(int chunkX, int chunkY, int x = 0, int y = 0);
  
  int getX() const noexcept; // Get the x coordinate of the Neighbourhood in its chunk. Return <= CHUNK_SIZE.
  int getY() const noexcept; // Get the y coordinate of the Neighbourhood in its chunk. Return <= CHUNK_SIZE.
  
  // Move the Neighbourhood one cell to the right in its chunk, updating the live count. Return the new x coordinate.
  // Throw std::range_error if the x coordinate is already CHUNK_SIZE and we try to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  void moveRight();
  
  // Move the Neighbourhood one cell to the left in its chunk, updating the live count.
  // Throw std::range_error if the x coordinate is already 0 and we try to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  void moveLeft();
  
  // Move the Neighbourhood one cell down in its chunk, updating the live count.
  // Throw std::range_error if the y coordinate is already CHUNK_SIZE and we try to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  void moveDown();
  
  // Move the Neighbourhood one cell up in its chunk, updating the live count.
  // Throw std::range_error if the y coordinate is already 0 and we try to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  void moveUp();
  
  // Move the Neighbourhood one cell towards the given side, updating the live count.
  // depending on which side is moved towards. Throw std::range_error if this call tries to move out of the chunk.
  // Throw std::logic_error if the Neighbourhood is not ready (i.e. moveTo has not been called).
  void moveToSide(const Side& side);
  
  // Get the live count of the Neighbourhood, i.e. the number of live cells in the Neighbourhood.
  unsigned int getLiveCount() const noexcept;
  
  virtual ~Neighbourhood() = default;
  
protected:
  // Initialize the Neighbourhood with the given ChunkArray. This Neighbourhood must be destroyed before the
  // ChunkArray passed here.
  explicit Neighbourhood(ChunkArray& chunkArray);
  
  // Check the Neighbourhood::ready_ flag and throw std::logic_error if it is not true.
  void verifyReady() const;
  
  // Take stock of the Neighbourhood's surroundings from scratch. Recreate the live count.
  // This is called from moveTo(int, int, int, int) to update from scratch.
  virtual void reinitialize() = 0;
  
  // These are the implementations of move[L/R/U/D], called after error checking and before changing x_ and y_.
  // They must update the live count, except for the centre cell. move[Left|Right|Down] takes care of that.
  virtual void translateRight() = 0;
  virtual void translateLeft() = 0;
  virtual void translateDown() = 0;
  virtual void translateUp() = 0;
  
  // Get the value of a cell by its offset from (x_, y_).
  bool getCell(int dx, int dy) const;
  
  int x_;
  int y_;
  int chunkX_;
  int chunkY_;
  unsigned int liveCount_;
  ChunkArray& chunkArray_;
  
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
  unsigned int getNumCells() const noexcept;
  
  // Get the maximum distance at which the centre cell in the Neighbourhood could possibly be affected by
  // a cell at this distance. Used to determine how far into empty chunks to evaluate - a cell in an empty chunk
  // is evaluated if both its x and y coordinates are no more than this far from a non-empty chunk.
  int getAffectingDistance() const noexcept;
  
  // Get the radius of the neighbourhood. This is the same as getAffectingDistance() but semantically different.
  int getRadius() const noexcept;
  
  // Clone method for polymorphic copying.
  virtual NeighbourhoodType* clone() const = 0;
  
  virtual ~NeighbourhoodType() = default; // prevent memory leaks! save lives!
  
protected:
  NeighbourhoodType(unsigned int numCells, int affectingDistance, int radius);
  const int radius_;
  
private:
  const unsigned int numCells_;
  const int affectingDistance_;
};

// MooreNeighbourhood

class MooreNeighbourhood; // forward declaration for MooreNeighbourhoodType

// The NeighbourhoodType representing MooreNeighbourhoods.
class MooreNeighbourhoodType : public NeighbourhoodType {
public:
  // Initialize a MooreNeighbourhoodType creating neighbourhoods with the specified radius.
  // Throw std::invalid_argument if radius is not positive.
  explicit MooreNeighbourhoodType(int radius);
  
  // Make a MooreNeighbourhood with the radius specified in the constructor.
  // (It's not covariant because of compiler weirdness, and we'll never use covariance anyways.)
  Neighbourhood* makeNeighbourhood(ChunkArray& chunkArray) const override;
  
  MooreNeighbourhoodType* clone() const override;
};

// A Moore neighbourhood includes diagonal adjacencies. It includes all cells in a square of side length 2r+1
// centred on the centre cell (except the centre cell itself), where r is the radius.
// Formally, if this neighbourhood is at (x, y), this neighbourhood includes all points (a, b), a != x and b != y, such
// that abs(a-x) <= r OR abs(b-y) <= r.
class MooreNeighbourhood : public Neighbourhood {
  friend Neighbourhood* MooreNeighbourhoodType::makeNeighbourhood(ChunkArray& chunkArray) const;
  
protected:
  // Initialize the MooreNeighbourhood with the given ChunkArray and radius.
  // Throw std::invalid_argument if the radius is negative or zero.
  MooreNeighbourhood(ChunkArray& chunkArray, int radius);
  
  // Regenerate the live count.
  void reinitialize() override;
  
  // Update the live count right, left, up, or down.
  void translateRight() override;
  void translateLeft() override;
  void translateDown() override;
  void translateUp() override;
  
private:
  const int radius_;
};

// VonNeumannNeighbourhood

class VonNeumannNeighbourhood; // forward declaration for VonNeumannNeighbourhoodType

// The NeighbourhoodType representing VonNeumannNeighbourhoods.
class VonNeumannNeighbourhoodType : public NeighbourhoodType {
public:
  // Initialize the VonNeumannNeighbourhoodType creating neighbourhoods with the specified radius.
  // Throw std::invalid_argument if the radius is negative or zero.
  explicit VonNeumannNeighbourhoodType(int radius);
  
  // Make a VonNeumannNeighbourhood with the radius specified in the constructor.
  Neighbourhood* makeNeighbourhood(ChunkArray& chunkArray) const override;
  
  VonNeumannNeighbourhoodType* clone() const override;
};

// A von Neumann neighbourhood does not include diagonal adjacencies. In my interpretation, it resembles a square on its
// corner of side length radius+1 centred on the centre cell. Formally, a von Neumann neighbourhood of radius r centred
// on (x, y) includes all points (a, b), a != x and b != y, such that abs(a-x) + abs(b-y) <= r.
class VonNeumannNeighbourhood : public Neighbourhood {
  friend Neighbourhood* VonNeumannNeighbourhoodType::makeNeighbourhood(ChunkArray& chunkArray) const;
  
protected:
  // Initialize the VonNeumannNeighbourhood with the given ChunkArray and radius.
  // Throw std::invalid_argument if the radius is negative or zero.
  VonNeumannNeighbourhood(ChunkArray& chunkArray, int radius);
  
  // Regenerate the live count.
  void reinitialize() override;
  
  // Update the live count left, right, up, or down.
  void translateRight() override;
  void translateLeft() override;
  void translateDown() override;
  void translateUp() override;
  
private:
  // Get the distance that the neighbourhood extends sideways in the a-th row or column.
  inline int getSideDist(int a);
  
  const int radius_;
};

#endif //GAME_OF_LIFE_NEIGHBOURHOOD_H
