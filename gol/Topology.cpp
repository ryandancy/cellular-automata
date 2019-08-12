#include <stdexcept>
#include "Topology.h"

// FixedTopology

FixedTopology::FixedTopology(unsigned int width, unsigned int height) : width(width), height(height) {
  if (width == 0 || height == 0) {
    throw std::out_of_range("Cannot create a FixedTopology with zero width or height");
  }
}

bool FixedTopology::valid(int x, int y) const {
  return x >= 0 && y >= 0 && x <= width && y <= height;
}

bool FixedTopology::transform(int& x, int& y) {
  // FixedTopology isn't going to actually transform it, it'll just declare it invalid if it's outside the limits
  return valid(x, y);
}
