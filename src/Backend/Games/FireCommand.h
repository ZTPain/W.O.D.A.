// FireCommand.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Coordinates.h"
#include "ICommand.h"

class FireCommand : public ICommand {
  GameBoard& board;
  Coordinates coords;

public:
  FireCommand(GameBoard& board, Coordinates coords);
  bool Execute() override;
  void Undo() override;
  [[nodiscard]] unsigned int ShotsHit() const override;
  [[nodiscard]] unsigned int UnitsDestroyed() const override;
};
