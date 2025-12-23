// MediumComputerStrategy.h

#pragma once

#include "Backend/Boards/SegmentBoard.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <cstddef>

class MediumComputerStrategy : public IComputerStrategy {
public:
  [[nodiscard]] Coordinates CalculateFireCoordinates(SegmentBoard& segmentBoard) const override;
};
