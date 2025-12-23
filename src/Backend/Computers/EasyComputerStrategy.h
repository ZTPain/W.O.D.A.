// EasyComputerStrategy.h

#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"

class EasyComputerStrategy : public IComputerStrategy {
public:
  [[nodiscard]] Coordinates CalculateFireCoordinates(ISegment& segmentBoard) const override;
};
