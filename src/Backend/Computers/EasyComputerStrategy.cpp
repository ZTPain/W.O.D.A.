#include "EasyComputerStrategy.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "ComputerStrategyHelper.h"
#include <cstddef>
#include <vector>

Coordinates EasyComputerStrategy::CalculateFireCoordinates(
    const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
) const {
  return ComputerStrategyHelper::ShootAtRandomCoordinate(board, blacklistedCoordinates);
}
