// SalvoFireCommand.cpp

#include "Backend/Games/SalvoFireCommand.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include <utility>
#include <vector>

SalvoFireCommand::SalvoFireCommand(GameBoard& board, std::vector<Coordinates> coords)
    : board(board), coords(std::move(coords)) {}

bool SalvoFireCommand::Execute() {
  unsigned short succesfulShotCount = 0;

  // Make all the shots and count them ...
  for (const auto& c : coords) {
    if (board.FireAt(c.x, c.y))
      succesfulShotCount++;
    else
      break;
  }

  // ... and if some were illegal, undo
  for (auto i = 0; i < succesfulShotCount; ++i)
    board.FixSegment(coords[i].x, coords[i].y);

  return succesfulShotCount == coords.size();
}

void SalvoFireCommand::Undo() {
  for (const auto& c : coords)
    board.FixSegment(c.x, c.y);
}
