// SalvoFireCommand.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Coordinates.h"
#include "ICommand.h"
#include <memory>
#include <vector>

class SalvoFireCommand : public ICommand {
  GameBoard* board;
  std::vector<Coordinates> coords;

public:
  SalvoFireCommand(const SalvoFireCommand& other);
  SalvoFireCommand(GameBoard* board, std::vector<Coordinates> coords);
  [[nodiscard]] std::unique_ptr<ICommand> Clone() const override;

  bool Execute() override;
  void Undo() override;
  [[nodiscard]] unsigned int Shots() const override;
  [[nodiscard]] unsigned int ShotsHit() const override;
  [[nodiscard]] unsigned int UnitsDestroyed() const override;
};
