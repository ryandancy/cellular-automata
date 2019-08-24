#ifndef GAME_OF_LIFE_CHUNKGRAPHICSITEM_H
#define GAME_OF_LIFE_CHUNKGRAPHICSITEM_H

#include <QBrush>
#include <QGraphicsItem>
#include <QObject>

#include "Chunk.h"

// TODO a singleton to hold graphics preferences (colours of background, live/dead cells)

// This GraphicsItem paints a Chunk's cells.
class ChunkGraphicsItem : public QObject, public QGraphicsItem {
  Q_OBJECT
  
public:
  static constexpr qreal SIZE = 150.0; // A chunk's size on-screen. A cell is ChunkGraphicsItem::SIZE / CHUNK_SIZE.
  
  // Initialize a ChunkGraphicsItem representing the specified Chunk. Connect the Chunk's signals to our slots.
  explicit ChunkGraphicsItem(const Chunk& chunk);
  
  QRectF boundingRect() const override;
  
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
  
public slots:
  void updateCells(bool newCells[CHUNK_SIZE][CHUNK_SIZE]);
  
private:
  const QBrush& liveBrush() const; // constant for the brush used for live cells
  const QBrush& deadBrush() const; // constant for the brush used for dead cells
  
  const int x_, y_; // the coordinates of the Chunk
  bool (* cells_)[CHUNK_SIZE]; // the Chunk's array of cells; for some reason it wants to be bool (*)[CHUNK_SIZE]?
  // we don't delete cells_ in a destructor because it's actually owned by the Chunk
};

#endif //GAME_OF_LIFE_CHUNKGRAPHICSITEM_H
