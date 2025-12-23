// IComputerStrategy.h

#pragma once

#include "Backend/Boards/SegmentBoard.h"
#include "Backend/Games/Coordinates.h"
#include <cstddef>

class IComputerStrategy {
public:
  virtual ~IComputerStrategy() = default;
  [[nodiscard]] virtual Coordinates CalculateFireCoordinates(SegmentBoard& segmentBoard) const = 0;
};
