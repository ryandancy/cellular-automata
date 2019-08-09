#include "Side.h"
#include <utility>

Side::operator Value() const {
  return value;
}

constexpr bool Side::operator==(const Side& side) const {
  return value == side.value;
}

constexpr bool Side::operator!=(const Side& side) const {
  return value != side.value;
}

void Side::transform(int& x, int& y, int width, int height) const {
  if (value == LEFT || value == RIGHT) {
    std::swap(x, y);
  }
  if (value == TOP || value == LEFT) {
    x = width - x;
  }
  if (value == TOP || value == RIGHT) {
    y = height - y;
  }
}
