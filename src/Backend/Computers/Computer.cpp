#include "Computer.h"
#include "EasyComputerStrategy.h"
#include "MediumComputerStrategy.h"
#include "HardComputerStrategy.h"

static EasyComputerStrategy easyStrategy;
static MediumComputerStrategy mediumStrategy;
static HardComputerStrategy hardStrategy;

Computer::Computer(ComputerType type)
    : computerStrategy(
        type == ComputerType::Medium ? static_cast<IComputerStrategy&>(mediumStrategy)
      : type == ComputerType::Hard   ? static_cast<IComputerStrategy&>(hardStrategy)
                                     : static_cast<IComputerStrategy&>(easyStrategy)) {}

Coordinates Computer::GetFireCoordinates(ISegment& board) {
    auto c = computerStrategy.CalculateFireCoordinates(board, *this);
    RememberShot(c.x, c.y);
    return c;
}

bool Computer::HasFiredAt(size_t x, size_t y) const {
    return shots.contains({x,y});
}

void Computer::RememberShot(size_t x, size_t y) {
    shots.insert({x,y});
}
