// HardComputerStrategy.h

#pragma once

#include "../Boards/GameBoard.h"
#include "../Games/Coordinates.h"
#include <cstddef>

template <size_t WIDTH, size_t HEIGHT> class HardComputerStrategy {
public:
  Coordinates CalculateFireCoordinates(const GameBoard<WIDTH, HEIGHT>& enemyBoard) const override;
};
