// HardComputerStrategy.h

#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"

class HardComputerStrategy : public IComputerStrategy {
public:
  [[nodiscard]] Coordinates CalculateFireCoordinates(ISegment& segmentBoard) const override;
};
