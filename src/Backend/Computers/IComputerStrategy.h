// IComputerStrategy.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include <vector>

class IComputerStrategy {
public:
  virtual ~IComputerStrategy() = default;
  [[nodiscard]] virtual Coordinates CalculateFireCoordinates(
      const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
  ) const = 0;
};
