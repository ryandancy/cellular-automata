#ifndef GAME_OF_LIFE_SIDE_H
#define GAME_OF_LIFE_SIDE_H


// Transforms coordinates relative to bottom to those relative to any side.
// This class is like an enum; use Side::TOP, Side::BOTTOM, Side::LEFT, and Side::RIGHT
class Side {
public:
  enum Value : unsigned int {
    BOTTOM, TOP, LEFT, RIGHT
  };
  constexpr Side(Value value) noexcept; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  operator Value() const noexcept { return value_; } // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  explicit operator bool() const = delete; // don't allow if (side)
  
  constexpr bool operator==(const Side& side) const;
  constexpr bool operator!=(const Side& side) const;
  
  // Transform x and y, relative to the bottom, to relative to this Side.
  void transform(int& x, int& y, int width, int height) const;
  
  // "Rotate" the side. Names are assuming that BOTTOM is the default, so clockwise is left.
  const Side& right() const noexcept; // Get the side obtained by rotating this side 90 degrees counterclockwise.
  const Side& left() const noexcept; // Get the side obtained by rotating this side 90 degrees clockwise.
  
  // These constants are to allow passing around Sides without constructing additional ones
  static const Side CONST_BOTTOM, CONST_TOP, CONST_LEFT, CONST_RIGHT;
  
private:
  Value value_;
};


#endif //GAME_OF_LIFE_SIDE_H
