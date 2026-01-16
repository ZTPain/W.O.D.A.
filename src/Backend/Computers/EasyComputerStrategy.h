// EasyComputerStrategy.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <vector>

class EasyComputerStrategy : public IComputerStrategy {
public:
  [[nodiscard]] Coordinates CalculateFireCoordinates(
      const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
  ) const override;
};
