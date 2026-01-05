// IComputerStrategy.h

#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"

class Computer;

class IComputerStrategy {
public:
    virtual ~IComputerStrategy() = default;
    [[nodiscard]]virtual Coordinates CalculateFireCoordinates(
        ISegment& segmentBoard,
        const Computer& self
    ) const = 0;
};
