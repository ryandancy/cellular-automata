#include "GraphicsProperties.h"

GraphicsProperties& GraphicsProperties::instance() {
  static GraphicsProperties singleton;
  return singleton;
}

GraphicsProperties::GraphicsProperties() : theme_(Theme::LIGHT) { // the default is light
  setTheme(Theme::LIGHT);
}

void GraphicsProperties::setTheme(Theme theme) {
  theme_ = theme;
  switch (theme) {
    case Theme::LIGHT:
      deadColor_ = Qt::white;
      liveColor_ = Qt::black;
      outOfBoundsColor_ = QColor(0xDDDDDD); // a light grey
      break;
    case Theme::DARK:
      deadColor_ = Qt::black;
      liveColor_ = Qt::white;
      outOfBoundsColor_ = QColor(0x333333); // a dark grey
      break;
    case Theme::HACKER:
      deadColor_ = Qt::black;
      liveColor_ = QColor(32, 194, 14); // "hacker green"
      outOfBoundsColor_ = QColor(0, 0x3A, 0); // a dark green
      break;
    case Theme::CANADIAN:
      deadColor_ = Qt::red;
      liveColor_ = Qt::white;
      outOfBoundsColor_ = QColor(0xA00000); // a light-ish red
      break;
    case Theme::VIOLET:
      deadColor_ = QColor(138, 43, 226); // "blue violet"
      liveColor_ = Qt::yellow;
      outOfBoundsColor_ = QColor(0x35166D); // a dark purple
      break;
  }
}

const QColor& GraphicsProperties::deadColor() const noexcept {
  return deadColor_;
}

const QColor& GraphicsProperties::liveColor() const noexcept {
  return liveColor_;
}

const QColor& GraphicsProperties::outOfBoundsColor() const noexcept {
  return outOfBoundsColor_;
}
