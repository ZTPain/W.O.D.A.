// FireCommand.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Coordinates.h"
#include "ICommand.h"
#include <memory>

class FireCommand : public ICommand {
  GameBoard& board;
  Coordinates coords;

public:
  FireCommand(GameBoard& board, Coordinates coords);
  [[nodiscard]] std::unique_ptr<ICommand> Clone() const override;

  bool Execute() override;
  void Undo() override;
  [[nodiscard]] unsigned int ShotsHit() const override;
  [[nodiscard]] unsigned int UnitsDestroyed() const override;
};
