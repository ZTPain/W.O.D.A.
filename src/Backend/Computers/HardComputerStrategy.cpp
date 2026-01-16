#include "HardComputerStrategy.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include "ComputerStrategyHelper.h"
#include <vector>

static bool ShouldCheat() { return ComputerStrategyHelper::GetRandomFromRange(0, 100) <= 20; }

Coordinates HardComputerStrategy::CalculateFireCoordinates(
    const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
) const {
  const auto finishTargets =
      ComputerStrategyHelper::GetShipsNotYetDestroyed(board, blacklistedCoordinates);
  if (!finishTargets.empty())
    return ComputerStrategyHelper::RandomFrom(finishTargets);

  if (ShouldCheat()) {
    const auto cheatTargets = ComputerStrategyHelper::Cheat(board, blacklistedCoordinates);
    if (!cheatTargets.empty())
      return ComputerStrategyHelper::RandomFrom(cheatTargets);
  }

  const auto chess =
      ComputerStrategyHelper::GetFreeChessboardCoordinates(board, blacklistedCoordinates);
  if (!chess.empty())
    return ComputerStrategyHelper::RandomFrom(chess);

  return ComputerStrategyHelper::ShootAtRandomCoordinate(board, blacklistedCoordinates);
}
