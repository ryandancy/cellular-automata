#ifndef GAME_OF_LIFE_AUTOMATONSCENE_H
#define GAME_OF_LIFE_AUTOMATONSCENE_H

#include <QGraphicsScene>
#include <QGraphicsSceneMouseEvent>
#include <QWidget>

#include "Automaton.h"

// The QGraphicsScene subclass which handles mouse events and formally represents the Automaton on-screen.
class AutomatonScene : public QGraphicsScene {
  Q_OBJECT
  
public:
  explicit AutomatonScene(Automaton& automaton, QWidget* parent = nullptr);
  
protected:
  void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
  
private:
  Automaton& automaton_; // we hold a reference to the automaton in order to add chunks
};

#endif //GAME_OF_LIFE_AUTOMATONSCENE_H
