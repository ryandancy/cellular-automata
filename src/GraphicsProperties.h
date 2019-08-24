#ifndef GAME_OF_LIFE_GRAPHICSPROPERTIES_H
#define GAME_OF_LIFE_GRAPHICSPROPERTIES_H

// A singleton to hold Qt graphics constants, but changeable.
class GraphicsProperties {
public:
  static GraphicsProperties& instance();
  
  GraphicsProperties(const GraphicsProperties&) = delete;
  GraphicsProperties(const GraphicsProperties&&) = delete;
  GraphicsProperties& operator=(const GraphicsProperties&) = delete;
  GraphicsProperties& operator=(const GraphicsProperties&&) = delete;
  
  bool showChunkBoxes = false;
  
private:
  GraphicsProperties() = default;
};

#endif //GAME_OF_LIFE_GRAPHICSPROPERTIES_H
