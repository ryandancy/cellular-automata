#include "GraphicsProperties.h"

GraphicsProperties& GraphicsProperties::instance() {
  static GraphicsProperties singleton;
  return singleton;
}
