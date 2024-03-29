#include <QPainter>

#include "ChunkGraphicsItem.h"
#include "GraphicsProperties.h"

constexpr qreal ChunkGraphicsItem::SIZE;

ChunkGraphicsItem::ChunkGraphicsItem(const Chunk& chunk) : x_(chunk.chunkX), y_(chunk.chunkY), cells_(nullptr) {
  // It's fine to initialize cells_ to nullptr since we treat nullptr as empty
  connect(&chunk, &Chunk::chunkChanged, this, &ChunkGraphicsItem::updateCells);
}

QRectF ChunkGraphicsItem::boundingRect() const {
  // Top-left is (x_*SIZE, y_*SIZE)
  return {x_ * SIZE, y_ * SIZE, SIZE, SIZE};
}

void ChunkGraphicsItem::paint(QPainter* painter, const QStyleOptionGraphicsItem*, QWidget*) {
  const QRectF& bounds = boundingRect();
  
  // draw all cells as dead first
  painter->setBrush(GraphicsProperties::instance().deadColor());
  if (GraphicsProperties::instance().showChunkBoxes) {
    painter->setPen(GraphicsProperties::instance().liveColor());
  } else {
    painter->setPen(Qt::transparent); // no outline
  }
  painter->drawRect(bounds);
  
  // treat nullptr as empty chunk (all dead)
  if (cells_ == nullptr) return;
  
  // draw live cells
  painter->setBrush(GraphicsProperties::instance().liveColor());
  painter->setPen(GraphicsProperties::instance().deadColor()); // cool cell separators!
  qreal cellSize = SIZE / CHUNK_SIZE;
  for (int x = 0; x < CHUNK_SIZE; x++) {
    for (int y = 0; y < CHUNK_SIZE; y++) {
      if (cells_[x][y]) {
        painter->drawRect(QRectF(bounds.x() + cellSize*x, bounds.y() + cellSize*y,
            cellSize, cellSize));
      }
    }
  }
}

void ChunkGraphicsItem::updateCells(bool (* newCells)[CHUNK_SIZE]) {
  cells_ = newCells; // wtf? why does newCells behave as bool(*)[C_S] when it's declared as bool[C_S][C_S]?
  update();
}
