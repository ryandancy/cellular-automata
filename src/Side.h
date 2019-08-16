#ifndef GAME_OF_LIFE_SIDE_H
#define GAME_OF_LIFE_SIDE_H


// Transforms coordinates relative to bottom to those relative to any side
// This class is like an enum; use Side::TOP, Side::BOTTOM, Side::LEFT, and Side::RIGHT
class Side {
public:
  enum Value {
    BOTTOM, TOP, LEFT, RIGHT
  };
  
  constexpr Side(Value value = BOTTOM); // NOLINT(google-explicit-constructor,hicpp-explicit-conversions)
  
  operator Value() const; // NOLINT(google-explicit-constructor,hicpp-explicit-conversions); allow switch
  explicit operator bool() const = delete; // don't allow let if (side)
  
  constexpr bool operator==(const Side& side) const;
  
  constexpr bool operator!=(const Side& side) const;
  
  // transforms x and y, relative to the bottom, to relative to this Side
  void transform(int& x, int& y, int width, int height) const;

private:
  Value value_;
};


#endif //GAME_OF_LIFE_SIDE_H
