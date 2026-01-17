// Computer.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <vector>

enum class ComputerType {
  None,
  Easy,
  Medium,
  Hard,
};

class Computer {
  IComputerStrategy* computerStrategy;
  ComputerType computerType;

public:
  Computer(ComputerType type);
  [[nodiscard]] Coordinates GetFireCoordinates(
      const GameBoard* board, const std::vector<Coordinates>& blacklistedCoordinates
  ) const;
  [[nodiscard]] ComputerType GetComputerType() const;
};
