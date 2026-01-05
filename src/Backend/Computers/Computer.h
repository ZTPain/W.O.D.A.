// Computer.h

#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include "IComputerStrategy.h"
#include <set>

enum class ComputerType {
    None,
    Easy,
    Medium,
    Hard,
};

class Computer {
    IComputerStrategy& computerStrategy;
    std::set<std::pair<size_t,size_t>> shots;

public:
    Computer(ComputerType type);

    Coordinates GetFireCoordinates(ISegment& board);

    bool HasFiredAt(size_t x, size_t y) const;
    void RememberShot(size_t x, size_t y);
};
