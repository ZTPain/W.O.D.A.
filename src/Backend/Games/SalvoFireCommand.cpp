// SalvoFireCommand.cpp

#include "Backend/Games/SalvoFireCommand.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include <utility>
#include <vector>

SalvoFireCommand::SalvoFireCommand(GameBoard& board, std::vector<Coordinates> coords)
    : board(board), coords(std::move(coords)) {}

bool SalvoFireCommand::Execute() { return true; }

void SalvoFireCommand::Undo() {}
