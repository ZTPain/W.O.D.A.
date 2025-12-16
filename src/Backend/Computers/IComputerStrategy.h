// IComputerStrategy.h

#pragma once

#include "../Boards/SegmentBoard.h"
#include "../Games/Coordinates.h"
#include <cstddef>

class IComputerStrategy {
public:
  virtual ~IComputerStrategy() = default;
  [[nodiscard]] virtual Coordinates CalculateFireCoordinates(SegmentBoard& segmentBoard) const = 0;
};
