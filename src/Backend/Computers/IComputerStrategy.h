// IComputerStrategy.h

#pragma once

#include "../Boards/SegmentBoardView.h"
#include "../Games/Coordinates.h"
#include <cstddef>

class IComputerStrategy {
public:
  virtual ~IComputerStrategy() = default;
  [[nodiscard]] virtual Coordinates CalculateFireCoordinates(SegmentBoardView view) const = 0;
};
