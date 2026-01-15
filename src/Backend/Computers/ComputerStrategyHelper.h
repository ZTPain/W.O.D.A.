#pragma once

#include "Backend/Boards/GameBoard.h"
#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include <cstddef>
#include <vector>

class ComputerStrategyHelper {
public:
  static size_t GetRandomFromRange(size_t min, size_t max);

  static bool IsCoordinateInList(
      const Coordinates& coord, const std::vector<Coordinates>& coordList
  );

  static Coordinates RandomFrom(const std::vector<Coordinates>& v);

  static bool IsValidShot(
      const ISegment& segmentBoard,
      const Coordinates& coord,
      const std::vector<Coordinates>& blacklistedCoordinates
  );

  static Coordinates ShootAtRandomCoordinate(
      const GameBoard& gameBoard, const std::vector<Coordinates>& blacklistedCoordinates
  );

  static std::vector<Coordinates> GetShipsNotYetDestroyed(
      const GameBoard& gameBoard, const std::vector<Coordinates>& blacklistedCoordinates
  );

  static std::vector<Coordinates> GetFreeChessboardCoordinates(
      const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
  );

  static std::vector<Coordinates> Cheat(
      const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
  );
};
