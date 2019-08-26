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
      break;
    case Theme::DARK:
      deadColor_ = Qt::black;
      liveColor_ = Qt::white;
      break;
    case Theme::HACKER:
      deadColor_ = Qt::black;
      liveColor_ = QColor(32, 194, 14); // "hacker green"
      break;
    case Theme::CANADIAN:
      deadColor_ = Qt::red;
      liveColor_ = Qt::white;
      break;
    case Theme::VIOLET:
      deadColor_ = QColor(138, 43, 226); // "blue violet"
      liveColor_ = Qt::yellow;
      break;
  }
}

GraphicsProperties::Theme GraphicsProperties::getTheme() const noexcept {
  return theme_;
}

const QColor& GraphicsProperties::deadColor() const noexcept {
  return deadColor_;
}

const QColor& GraphicsProperties::liveColor() const noexcept {
  return liveColor_;
}
