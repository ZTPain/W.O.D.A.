#include "Computer.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "EasyComputerStrategy.h"
#include "HardComputerStrategy.h"
#include "IComputerStrategy.h"
#include "MediumComputerStrategy.h"
#include <stdexcept>
#include <vector>

static EasyComputerStrategy easyStrategy;
static MediumComputerStrategy mediumStrategy;
static HardComputerStrategy hardStrategy;

Computer::Computer(ComputerType type) : computerType(type) {
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

Coordinates Computer::GetFireCoordinates(
    const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
) const {
  return computerStrategy->CalculateFireCoordinates(board, blacklistedCoordinates);
}

ComputerType Computer::GetComputerType() const { return computerType; }
