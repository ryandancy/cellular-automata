#include "Side.h"
#include <utility>

const Side Side::CONST_BOTTOM(Side::BOTTOM);
const Side Side::CONST_TOP(Side::TOP);
const Side Side::CONST_LEFT(Side::LEFT);
const Side Side::CONST_RIGHT(Side::RIGHT);

constexpr Side::Side(Value value) noexcept : value_(value) {}

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
    x = width - x - 1;
  }
  if (value_ == TOP || value_ == RIGHT) {
    y = height - y - 1;
  }
}

const Side& Side::right() const noexcept {
  switch (value_) {
    case BOTTOM:
    default:
      return Side::CONST_RIGHT;
    case LEFT:
      return Side::CONST_BOTTOM;
    case TOP:
      return Side::CONST_LEFT;
    case RIGHT:
      return Side::CONST_TOP;
  }
}

const Side& Side::left() const noexcept {
  switch (value_) {
    case BOTTOM:
    default:
      return Side::CONST_LEFT;
    case RIGHT:
      return Side::CONST_BOTTOM;
    case TOP:
      return Side::CONST_RIGHT;
    case LEFT:
      return Side::CONST_TOP;
  }
}
