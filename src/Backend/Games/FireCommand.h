// FireCommand.h

#pragma once

#include "../Boards/GameBoard.h"
#include "Coordinates.h"
#include "ICommand.h"
#include <cstddef>

class FireCommand : ICommand {
  GameBoard& board;
  Coordinates coords;

public:
  FireCommand(GameBoard& board, Coordinates coords);
  bool Execute() override;
  void Undo() override;
};
