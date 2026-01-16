// SalvoFireCommand.cpp

#include "Backend/Games/SalvoFireCommand.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "Backend/Games/ICommand.h"
#include <memory>
#include <utility>
#include <vector>

SalvoFireCommand::SalvoFireCommand(GameBoard& board, std::vector<Coordinates> coords)
    : board(board), coords(std::move(coords)) {}

SalvoFireCommand::SalvoFireCommand(const SalvoFireCommand& other)
    : board(other.board), coords(other.coords) {}

bool SalvoFireCommand::Execute() {
  unsigned short succesfulShotCount = 0;

  // Make all the shots and count them ...
  for (const auto& c : coords) {
    if (board.FireAt(c.x, c.y))
      succesfulShotCount++;
    else
      break;
  }

  if (succesfulShotCount == coords.size())
    return true;

  // ... and if some were illegal, undo
  for (auto i = 0; i < succesfulShotCount; ++i)
    board.FixSegment(coords[i].x, coords[i].y);

  return false;
}

void SalvoFireCommand::Undo() {
  for (const auto& c : coords)
    board.FixSegment(c.x, c.y);
}

std::unique_ptr<ICommand> SalvoFireCommand::Clone() const {
  return std::make_unique<SalvoFireCommand>(*this);
}
