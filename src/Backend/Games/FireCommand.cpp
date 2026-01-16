// FireCommand.cpp

#include "Backend/Games/FireCommand.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"

FireCommand::FireCommand(GameBoard& board, Coordinates coords) : board(board), coords(coords) {}

bool FireCommand::Execute() { return board.FireAt(coords.x, coords.y); }

void FireCommand::Undo() { board.FixSegment(coords.x, coords.y); }

unsigned int FireCommand::ShotsHit() const {
  return board.Units()[coords.y][coords.x] != nullptr ? 1 : 0;
}

unsigned int FireCommand::UnitsDestroyed() const {
  if (board.Units()[coords.y][coords.x] == nullptr)
    return 0;
  return board.Units()[coords.y][coords.x]->IsDestroyed() ? 1 : 0;
}
