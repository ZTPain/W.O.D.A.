// MediumComputerStrategy.h

#pragma once

#include "../Boards/SegmentBoard.h"
#include "../Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <cstddef>

class MediumComputerStrategy : public IComputerStrategy {
public:
  [[nodiscard]] Coordinates CalculateFireCoordinates(SegmentBoard& segmentBoard) const override;
};
