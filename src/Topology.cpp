#include <stdexcept>
#include "Topology.h"

// BoundedTopology

BoundedTopology::BoundedTopology(int width, int height) : width_(width), height_(height) {
  if (width <= 0 || height <= 0) {
    throw std::out_of_range("Cannot create a BoundedTopology with negative or zero width_ or height_");
  }
}

bool BoundedTopology::valid(int x, int y) const {
  return x >= 0 && y >= 0 && x <= width_ && y <= height_;
}

// FixedTopology

bool FixedTopology::transform(int& x, int& y) const {
  // FixedTopology isn't going to actually transform it, it'll just declare it invalid if it's outside the limits
  return valid(x, y);
}

// WrappingTopology

bool WrappingTopology::transform(int& x, int& y) const {
  // wrapping is accomplished with modulus
  x %= width_;
  if (x < 0) x += width_;
  y %= height_;
  if (y < 0) y += height_;
  return true;
}

// UnboundedTopology

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wunused-parameter" // x and y aren't used in UnboundedTopology

bool UnboundedTopology::valid(int x, int y) const {
  return true; // all chunks are valid
}

bool UnboundedTopology::transform(int& x, int& y) const {
  return true; // no transformations are performed since all chunks are valid
}

#pragma clang diagnostic pop
