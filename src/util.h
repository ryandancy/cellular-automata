#ifndef GAME_OF_LIFE_UTIL_H
#define GAME_OF_LIFE_UTIL_H

#include <utility>
#include <functional>

// template hash function for std::pair for use in maps, works with all types defined in <functional>
struct pair_hash {
  template<typename A, typename B>
  std::size_t operator()(const std::pair<A, B>& pair) const {
    std::size_t hash = std::hash<A>()(pair.first);
    // this magic is used to combine the hashes from the first and second elements
    // apparently, it's better than a simple xor; this implementation is from boost
    return hash ^ (std::hash<B>()(pair.second) + 0x9e3779b9 + (hash<<6u) + (hash>>2u));
  }
};

#endif //GAME_OF_LIFE_UTIL_H
