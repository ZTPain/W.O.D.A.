// SalvoFireCommand.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Coordinates.h"
#include "ICommand.h"
#include <vector>

class SalvoFireCommand : public ICommand {
  GameBoard& board;
  std::vector<Coordinates> coords;

public:
  SalvoFireCommand(GameBoard& board, std::vector<Coordinates> coords);
  bool Execute() override;
  void Undo() override;
  [[nodiscard]] unsigned int ShotsHit() const override;
  [[nodiscard]] unsigned int UnitsDestroyed() const override;
};
