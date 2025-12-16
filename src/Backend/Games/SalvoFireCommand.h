// SalvoFireCommand.h

#pragma once

#include "../Boards/GameBoard.h"
#include "Coordinates.h"
#include "ICommand.h"
#include <cstddef>
#include <vector>

class SalvoFireCommand : ICommand {
  GameBoard& board;
  std::vector<Coordinates> coords;

public:
  SalvoFireCommand(GameBoard& board, std::vector<Coordinates> coords);
  bool Execute() override;
  void Undo() override;
};
