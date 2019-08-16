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

// This is an abstract Topology with bounds, meaning it has a width and a height. Chunks are valid iff they are within
// (0, 0) and (width, height), inclusive.
class BoundedTopology : public Topology {
public:
  // Initialize the bounds with a width and height, in units of chunks.
  // Will throw std::out_of_range if the width or height are negative or zero.
  BoundedTopology(int width, int height);
  
  // Simply return whether 0 <= x <= width and 0 <= y <= height.
  bool valid(int x, int y) const override;
  
  // Note: width and height are signed to prevent headaches with arithmetic with signed coordinates
  const int width_; // The width of the boundary of the topology, in chunks
  const int height_; // The height of the boundary of the topology, in chunks
};

// This is a Topology with a fixed size - it simply stops after a certain number of Chunks.
// All other chunks should be treated as completely empty/dead.
class FixedTopology : public BoundedTopology {
  using BoundedTopology::BoundedTopology;
  
public:
  // If (x, y) is valid, this method will do nothing and return true. If not, it will return false, signifying
  // that the chunk should be treated as fully empty.
  bool transform(int& x, int& y) override;
};

// This is a Topology which wraps around in both the x and y directions. Topologically, it forms a torus.
class WrappingTopology : public BoundedTopology {
  using BoundedTopology::BoundedTopology;
  
public:
  // This implementation will wrap x and y as many times as necessary. It will always return true.
  bool transform(int& x, int& y) override;
};

// This is a Topology with no restrictions whatsoever. All chunks are valid and no transformations are performed.
class UnboundedTopology : public Topology {
public:
  bool valid(int x, int y) const override; // Always returns true - all chunks are valid.
  bool transform(int& x, int& y) override; // Always returns true and does not modify x and y.
};

#endif //GAME_OF_LIFE_TOPOLOGY_H
