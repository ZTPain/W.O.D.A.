// Computer.h

#pragma once

#include "Backend/Boards/SegmentBoard.h"
#include "Backend/Games/Coordinates.h"
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
  [[nodiscard]] Coordinates GetFireCoordinates(SegmentBoard& segmentBoard) const;
};
