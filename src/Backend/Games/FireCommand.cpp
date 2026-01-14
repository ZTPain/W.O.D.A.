// FireCommand.cpp

#include "Backend/Games/FireCommand.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/ICommand.h"
#include <memory>

FireCommand::FireCommand(GameBoard& board, Coordinates coords) : board(board), coords(coords) {}

bool FireCommand::Execute() { return board.FireAt(coords.x, coords.y); }

void FireCommand::Undo() { board.FixSegment(coords.x, coords.y); }

std::unique_ptr<ICommand> FireCommand::Clone() const {
  return std::make_unique<FireCommand>(*this);
}
