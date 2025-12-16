// Computer.h

#pragma once

#include "../Boards/GameBoard.h"
#include "../Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <cstddef>

enum class ComputerType {
  None,
  Easy,
  Medium,
  Hard,
};

template <size_t WIDTH, size_t HEIGHT> class Computer {
  IComputerStrategy<WIDTH, HEIGHT> computerStrategy;

public:
  Computer(ComputerType type);
  Coordinates GetFireCoordinates(const GameBoard<WIDTH, HEIGHT>& enemyBoard) const;
};
