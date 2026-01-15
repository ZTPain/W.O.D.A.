#include "MediumComputerStrategy.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "ComputerStrategyHelper.h"
#include <vector>

Coordinates MediumComputerStrategy::CalculateFireCoordinates(
    const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
) const {
  const auto finishTargets =
      ComputerStrategyHelper::GetShipsNotYetDestroyed(board, blacklistedCoordinates);
  if (!finishTargets.empty())
    return ComputerStrategyHelper::RandomFrom(finishTargets);

  const auto chess =
      ComputerStrategyHelper::GetFreeChessboardCoordinates(board, blacklistedCoordinates);
  if (!chess.empty())
    return ComputerStrategyHelper::RandomFrom(chess);

  return ComputerStrategyHelper::ShootAtRandomCoordinate(board, blacklistedCoordinates);
}
