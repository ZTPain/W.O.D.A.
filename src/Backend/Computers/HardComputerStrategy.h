// HardComputerStrategy.h

#pragma once

#include "../Boards/SegmentBoardView.h"
#include "../Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <cstddef>

class HardComputerStrategy : public IComputerStrategy {
public:
  [[nodiscard]] Coordinates CalculateFireCoordinates(SegmentBoardView view) const override;
};
