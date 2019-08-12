#ifndef GAME_OF_LIFE_TOPOLOGY_H
#define GAME_OF_LIFE_TOPOLOGY_H

// A Topology represents how Chunks are related to each other in space. It defines valid chunks and allows
// transformation of invalid chunk coordinates into their equivalent valid ones - i.e. which chunk a chunk with
// invalid coordinates should behave like for the purpose of interacting with other chunks.
// All coordinates in this class are given in terms of chunk coordinate, *not* cell coordinates.
class Topology {
public:
  // Is (x, y) a valid set of coordinates for a real Chunk in this Topology?
  virtual bool valid(int x, int y) const = 0;
  
  // Transform a possibly-invalid set of coordinates (x, y) to a valid set and return true, or return false if this
  // cannot be done and the chunk should be treated as fully empty.
  virtual bool transform(int& x, int& y) = 0;
};

// This is a Topology with a fixed size - it simply stops after a certain number of Chunks.
// All other chunks should be treated as completely empty/dead.
class FixedTopology : public Topology {
public:
  // Initialize with a width and height, in units of chunks.
  // Will throw std::out_of_range if the width or height is 0.
  FixedTopology(unsigned int width, unsigned int height);
  
  // This implementation simply returns whether if 0 <= x <= width and 0 <= y <= height.
  bool valid(int x, int y) const override;
  
  // If (x, y) is valid, this method will do nothing and return true. If not, it will return false, signifying
  // that the chunk should be treated as fully empty.
  bool transform(int& x, int& y) override;
  
  const unsigned int width; // The width of the fixed boundary of the topology, in chunks
  const unsigned int height; // The height of the fixed boundary of the topology, in chunks
};

#endif //GAME_OF_LIFE_TOPOLOGY_H
