#include <stdexcept>
#include "Topology.h"

// Topology (default unbounded) - TODO can we design this any better to avoid the weird overriding thing?
// This design is the easiest to build on top of BoundedTopology but maybe we should know better

bool Topology::bounded() const noexcept {
  return false; // default unbounded
}

int Topology::width() const noexcept {
  return -1;
}

int Topology::height() const noexcept {
  return -1;
}

// BoundedTopology

BoundedTopology::BoundedTopology(int width, int height) : width_(width), height_(height) {
  if (width <= 0 || height <= 0) {
    throw std::out_of_range("Cannot create a BoundedTopology with negative or zero width or height");
  }
}

bool BoundedTopology::valid(int x, int y) const {
  return x >= 0 && y >= 0 && x < width_ && y < height_;
}

bool BoundedTopology::bounded() const noexcept {
  return true;
}

int BoundedTopology::width() const noexcept {
  return width_;
}

int BoundedTopology::height() const noexcept {
  return height_;
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

bool UnboundedTopology::valid(int, int) const {
  return true; // all chunks are valid
}

bool UnboundedTopology::transform(int&, int&) const {
  return true; // no transformations are performed since all chunks are valid
}
