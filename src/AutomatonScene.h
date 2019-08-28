#ifndef GAME_OF_LIFE_AUTOMATONSCENE_H
#define GAME_OF_LIFE_AUTOMATONSCENE_H

#include <QGraphicsRectItem>
#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QWidget>

#include "Automaton.h"

// The QGraphicsScene subclass which handles mouse events and formally represents the Automaton on-screen.
class AutomatonScene : public QGraphicsScene {
  Q_OBJECT
  
public:
  // Initialize with the given automaton pointer. We do not own this pointer. Update it with updateAutomaton(Automaton*)
  // whenever it is invalidated.
  explicit AutomatonScene(Automaton* automaton, QWidget* parent = nullptr);
  ~AutomatonScene() override;
  
  // Update the background colour (and the topology edge colour).
  void updateBackground();
  
  // Change the old automaton's reference for this one and update. Use when the old automaton pointer was invalidated.
  void updateAutomaton(Automaton* automaton);
  
protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  
private:
  Automaton* automaton_; // we hold a pointer to the automaton in order to add chunks, but don't own it
  QGraphicsRectItem* validRect_ = nullptr; // the rectangle where cells are valid; we own this
};

#endif //GAME_OF_LIFE_AUTOMATONSCENE_H
