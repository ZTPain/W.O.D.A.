// IComputerStrategy.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"

class IComputerStrategy {
public:
  virtual ~IComputerStrategy() = default;
  [[nodiscard]] virtual Coordinates CalculateFireCoordinates(const GameBoard& board) const = 0;
};
