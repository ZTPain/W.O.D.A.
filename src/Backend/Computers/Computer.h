// Computer.h

#pragma once

#include "../Boards/SegmentBoardView.h"
#include "../Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <cstddef>

enum class ComputerType {
  None,
  Easy,
  Medium,
  Hard,
};

class Computer {
  IComputerStrategy& computerStrategy;

public:
  Computer(ComputerType type);
  [[nodiscard]] Coordinates GetFireCoordinates(SegmentBoardView view) const;
};
