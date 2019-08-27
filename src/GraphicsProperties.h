#ifndef GAME_OF_LIFE_GRAPHICSPROPERTIES_H
#define GAME_OF_LIFE_GRAPHICSPROPERTIES_H

#include <QColor>

// A singleton to hold Qt graphics constants, but changeable.
class GraphicsProperties {
public:
  static GraphicsProperties& instance();
  
  GraphicsProperties(const GraphicsProperties&) = delete;
  GraphicsProperties(const GraphicsProperties&&) = delete;
  GraphicsProperties& operator=(const GraphicsProperties&) = delete;
  GraphicsProperties& operator=(const GraphicsProperties&&) = delete;
  
  bool showChunkBoxes = false;
  
  enum class Theme {
    LIGHT, DARK, HACKER, CANADIAN, VIOLET
  };
  
  void setTheme(Theme theme);
  
  const QColor& deadColor() const noexcept; // the colour of dead cells and the background
  const QColor& liveColor() const noexcept; // the colour of live cells
  const QColor& outOfBoundsColor() const noexcept; // the colour of out-of-bound chunks
  
private:
  GraphicsProperties();
  
  Theme theme_;
  QColor deadColor_;
  QColor liveColor_;
  QColor outOfBoundsColor_;
};

#endif //GAME_OF_LIFE_GRAPHICSPROPERTIES_H
