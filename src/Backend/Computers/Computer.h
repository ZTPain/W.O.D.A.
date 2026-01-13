// Computer.h

#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"

enum class ComputerType {
  None,
  Easy,
  Medium,
  Hard,
};

class Computer {
  IComputerStrategy* computerStrategy;
  ComputerType computerType;

public:
  Computer(ComputerType type);
  [[nodiscard]] Coordinates GetFireCoordinates(ISegment& segmentBoard) const;
  [[nodiscard]] ComputerType GetComputerType() const;
};
