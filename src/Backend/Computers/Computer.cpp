#include "Computer.h"
#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include "EasyComputerStrategy.h"
#include "HardComputerStrategy.h"
#include "IComputerStrategy.h"
#include "MediumComputerStrategy.h"
#include <stdexcept>

static EasyComputerStrategy easyStrategy;
static MediumComputerStrategy mediumStrategy;
static HardComputerStrategy hardStrategy;

Computer::Computer(ComputerType type) {
  switch (type) {
    case ComputerType::Easy:
      computerStrategy = &easyStrategy;
      break;
    case ComputerType::Medium:
      computerStrategy = &mediumStrategy;
      break;
    case ComputerType::Hard:
      computerStrategy = &hardStrategy;
      break;
    default:
      throw std::invalid_argument("Invalid ComputerType");
  }
}

Coordinates Computer::GetFireCoordinates(ISegment& segmentBoard) const {
  return computerStrategy->CalculateFireCoordinates(segmentBoard);
}
