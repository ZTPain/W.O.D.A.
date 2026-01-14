// MediumComputerStrategy.h

#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"

class MediumComputerStrategy : public IComputerStrategy {
public:
  [[nodiscard]] Coordinates CalculateFireCoordinates(const GameBoard& board) const override;
};
