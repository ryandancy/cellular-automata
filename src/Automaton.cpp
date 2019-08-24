#include <stdexcept>

#include "Automaton.h"

Automaton::Automaton(Topology* topology, NeighbourhoodType* initialNeighbourhoodType)
    : chunkArray_(topology), ruleset_(initialNeighbourhoodType), generation_(0) {
  if (topology == nullptr || initialNeighbourhoodType == nullptr) {
    throw std::invalid_argument("Cannot initialize Automaton with a null pointer");
  }
}

void Automaton::setNeighbourhoodType(NeighbourhoodType* neighbourhoodType) {
  if (neighbourhoodType == nullptr) {
    throw std::invalid_argument("Cannot set automaton's neighbourhood type to a null pointer");
  }
  ruleset_.setNeighbourhoodType(neighbourhoodType);
}

void Automaton::tick() {
  // This is the one Neighbourhood for this tick - smart pointer for exception safety
  std::unique_ptr<Neighbourhood> neighbourhood(ruleset_.getNeighbourhoodType().makeNeighbourhood(chunkArray_));
  int affectingDistance = ruleset_.getNeighbourhoodType().getAffectingDistance();
  
  // Generate for every chunk
  for (auto& chunkPair : chunkArray_) {
    int x = chunkPair.first.first, y = chunkPair.first.second;
    Chunk& chunk = *chunkPair.second;
    
    if (chunk.isEmpty()) {
      // Only generate on sides where it's possible to affect something
      bool left = chunkArray_.hasNonEmpty(x-1, y),
          right = chunkArray_.hasNonEmpty(x+1, y),
          top = chunkArray_.hasNonEmpty(x, y-1),
          bottom = chunkArray_.hasNonEmpty(x, y+1),
          leftTop = chunkArray_.hasNonEmpty(x-1, y-1),
          leftBottom = chunkArray_.hasNonEmpty(x-1, y+1),
          rightTop = chunkArray_.hasNonEmpty(x+1, y-1),
          rightBottom = chunkArray_.hasNonEmpty(x+1, y+1);
      
      if (left || leftTop || leftBottom) {
        chunk.generate(ruleset_, *neighbourhood, Side::LEFT, affectingDistance);
      }
      if (right || rightTop || rightBottom) {
        chunk.generate(ruleset_, *neighbourhood, Side::RIGHT, affectingDistance);
      }
      if (top || leftTop || rightTop) {
        chunk.generate(ruleset_, *neighbourhood, Side::TOP, affectingDistance);
      }
      if (bottom || leftBottom || rightBottom) {
        chunk.generate(ruleset_, *neighbourhood, Side::BOTTOM, affectingDistance);
      }
    } else {
      // Generate the entire chunk
      chunk.generate(ruleset_, *neighbourhood);
    }
  }
  
  // Update every chunk
  for (auto& chunkPair : chunkArray_) {
    chunkPair.second->update();
  }
  
  // Insert the queued chunks here so we can remove useless ones
  chunkArray_.insertAllInQueue();
  
  // Remove isolated empty chunks and add empty chunks beside non-padded non-empty ones
  // (Iterator idiom is to avoid getting errors for modifying chunkArray_ while iterating)
  for (auto it = chunkArray_.cbegin(), nextIt = it; it != chunkArray_.cend(); it = nextIt) {
    ++nextIt;
    
    int x = it->first.first, y = it->first.second;
    Chunk& chunk = *it->second;
    
    if (chunk.isEmpty()) {
      // See if we can erase it
      bool hasAny = false;
      for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
          if (dx == 0 && dy == 0) continue;
          if (chunkArray_.hasNonEmpty(x+dx, y+dy)) {
            hasAny = true;
            break;
          }
        }
        if (hasAny) break;
      }
    
      if (!hasAny) {
        chunkArray_.erase(x, y);
      }
    } else {
      // Do we need to add any?
      // TODO it's possible that this isn't necessary because Neighbourhood checks this - test
      for (int dx = -1; dx <= 1; dx++) {
        for (int dy = -1; dy <= 1; dy++) {
          if (dx == 0 && dy == 0) continue;
          chunkArray_.insertOrNoop(x + dx, y + dy); // does nothing if present
        }
      }
    }
  }
  
  generation_++; // we've accomplished something
}

int Automaton::generation() const noexcept {
  return generation_;
}

ChunkArray& Automaton::chunkArray() noexcept {
  return chunkArray_;
}

Ruleset& Automaton::ruleset() noexcept {
  return ruleset_;
}
