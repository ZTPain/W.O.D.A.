#include "Computer.h"
#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include "EasyComputerStrategy.h"
#include "HardComputerStrategy.h"
#include "IComputerStrategy.h"
#include "MediumComputerStrategy.h"

namespace {
EasyComputerStrategy easyStrategy;
MediumComputerStrategy mediumStrategy;
HardComputerStrategy hardStrategy;
} // namespace

Computer::Computer(ComputerType type)
    : computerStrategy(
          type == ComputerType::Medium ? static_cast<IComputerStrategy&>(mediumStrategy)
          : type == ComputerType::Hard ? static_cast<IComputerStrategy&>(hardStrategy)
                                       : static_cast<IComputerStrategy&>(easyStrategy)
      ) {}

Coordinates Computer::GetFireCoordinates(ISegment& segmentBoard) const {
  return computerStrategy.CalculateFireCoordinates(segmentBoard);
}
