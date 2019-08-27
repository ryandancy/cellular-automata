#include <QGraphicsRectItem>
#include <QPointF>

#include "AutomatonScene.h"
#include "Chunk.h"
#include "ChunkGraphicsItem.h"
#include "GraphicsProperties.h"

AutomatonScene::AutomatonScene(Automaton& automaton, QWidget* parent) : QGraphicsScene(parent), automaton_(automaton) {
  updateBackground();
}

void AutomatonScene::updateBackground() {
  if (automaton_.chunkArray().topology().bounded()) {
    setBackgroundBrush(GraphicsProperties::instance().outOfBoundsColor());
    auto validRect = new QGraphicsRectItem(0.0, 0.0,
        ChunkGraphicsItem::SIZE * automaton_.chunkArray().topology().width(),
        ChunkGraphicsItem::SIZE * automaton_.chunkArray().topology().height());
    validRect->setBrush(GraphicsProperties::instance().deadColor());
    validRect->setPen(Qt::NoPen);
    validRect->setZValue(-1.0); // behind all the default z 0.0 stuff
    addItem(validRect);
  } else {
    setBackgroundBrush(GraphicsProperties::instance().deadColor());
  }
}

void AutomatonScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* event) {
  // Flip the cell with a left click
  if (event->button() != Qt::LeftButton) return;
  
  const QPointF& pos = event->scenePos();
  
  // Calculate chunk and cell positions from scenePos
  // hopefully this takes into account scroll position? also hopefully we can scroll?
  int chunkX = (int) (pos.x() / ChunkGraphicsItem::SIZE);
  int chunkY = (int) (pos.y() / ChunkGraphicsItem::SIZE);
  
  qreal relCellX = pos.x() - chunkX*ChunkGraphicsItem::SIZE;
  qreal relCellY = pos.y() - chunkY*ChunkGraphicsItem::SIZE;
  if (relCellX < 0) {
    relCellX += ChunkGraphicsItem::SIZE;
    chunkX--;
  }
  if (relCellY < 0) {
    relCellY += ChunkGraphicsItem::SIZE;
    chunkY--;
  }
  int cellX = (int) (relCellX / (ChunkGraphicsItem::SIZE / CHUNK_SIZE));
  int cellY = (int) (relCellY / (ChunkGraphicsItem::SIZE / CHUNK_SIZE));
  
  // Flip the cell in the chunk, adding it if it doesn't exist
  automaton_.chunkArray().insertOrNoop(chunkX, chunkY);
  Chunk& chunk = automaton_.chunkArray().at(chunkX, chunkY);
  chunk.setCell(cellX, cellY, !chunk.getCell(cellX, cellY));
}
