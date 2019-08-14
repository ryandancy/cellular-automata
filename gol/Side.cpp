#include "Side.h"
#include <utility>

constexpr Side::Side(Value value) : value_(value) {}

Side::operator Value() const {
  return value_;
}

constexpr bool Side::operator==(const Side& side) const {
  return value_ == side.value_;
}

constexpr bool Side::operator!=(const Side& side) const {
  return value_ != side.value_;
}

void Side::transform(int& x, int& y, int width, int height) const {
  if (value_ == LEFT || value_ == RIGHT) {
    std::swap(x, y);
  }
  if (value_ == TOP || value_ == LEFT) {
    x = width - x;
  }
  if (value_ == TOP || value_ == RIGHT) {
    y = height - y;
  }
}
