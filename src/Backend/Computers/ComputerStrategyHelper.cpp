#include "ComputerStrategyHelper.h"

#include "Backend/Boards/GameBoard.h"
#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <random>
#include <utility>
#include <vector>

size_t ComputerStrategyHelper::GetRandomFromRange(size_t min, size_t max) {
  static std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<size_t> dist(min, max);
  return dist(rng);
}

bool ComputerStrategyHelper::IsCoordinateInList(
    const Coordinates& coord, const std::vector<Coordinates>& coordList
) {
  return std::any_of(coordList.begin(), coordList.end(), [&coord](const Coordinates& c) {
    return c.x == coord.x && c.y == coord.y;
  });
}

Coordinates ComputerStrategyHelper::RandomFrom(const std::vector<Coordinates>& v) {
  if (v.empty())
    return Coordinates{SIZE_MAX, SIZE_MAX}; // Return invalid coordinate if vector is empty

  return v[GetRandomFromRange(0, v.size() - 1)];
}

bool ComputerStrategyHelper::IsValidShot(
    const ISegment& segmentBoard,
    const Coordinates& coord,
    const std::vector<Coordinates>& blacklistedCoordinates
) {
  const size_t w = segmentBoard.Width();
  const size_t h = segmentBoard.Height();

  if (coord.x >= w || coord.y >= h)
    return false;

  if (segmentBoard.Segments()[coord.y][coord.x])
    return false;

  if (IsCoordinateInList(coord, blacklistedCoordinates))
    return false;

  return true;
}

Coordinates ComputerStrategyHelper::ShootAtRandomCoordinate(
    const GameBoard& gameBoard, const std::vector<Coordinates>& blacklistedCoordinates
) {
  const size_t w = gameBoard.Width();
  const size_t h = gameBoard.Height();
  const auto& segments = gameBoard.GetSegmentBoard().Segments();
  std::vector<Coordinates> pool;

  for (size_t y = 0; y < h; y++) {
    for (size_t x = 0; x < w; x++) {
      Coordinates coord{x, y};
      if (!segments[y][x] && !IsCoordinateInList(coord, blacklistedCoordinates)) {
        pool.emplace_back(coord);
      }
    }
  }

  return RandomFrom(pool);
}

std::vector<Coordinates> ComputerStrategyHelper::GetShipsNotYetDestroyed(
    const GameBoard& gameBoard, const std::vector<Coordinates>& blacklistedCoordinates
) {
  const auto& segmentBoard = gameBoard.GetSegmentBoard();
  const auto& shots = segmentBoard.Segments();
  const auto& units = gameBoard.Units();
  const size_t w = gameBoard.Width();
  const size_t h = gameBoard.Height();

  std::vector<Coordinates> pool;

  for (size_t y = 0; y < h; y++) {
    for (size_t x = 0; x < w; x++) {
      const auto u = units[y][x];
      if (!u)
        continue;

      if (!shots[y][x] || u->IsDestroyed())
        continue;

      static constexpr const std::array<std::pair<int, int>, 4> DIRS{
          {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
      };

      for (const auto [dx, dy] : DIRS) {
        const size_t nx = x + dx;
        const size_t ny = y + dy;

        const Coordinates coord{nx, ny};
        if (!IsValidShot(segmentBoard, coord, blacklistedCoordinates))
          continue;

        pool.emplace_back(coord);
      }
    }
  }

  return pool;
}

std::vector<Coordinates> ComputerStrategyHelper::GetFreeChessboardCoordinates(
    const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
) {
  std::vector<Coordinates> out;
  const auto& s = board.GetSegmentBoard().Segments();
  for (size_t y = 0; y < board.Height(); ++y) {
    for (size_t x = 0; x < board.Width(); ++x) {
      if ((x + y) % 2 != 0)
        continue;

      if (s[y][x])
        continue;

      const Coordinates coord{x, y};

      if (IsCoordinateInList(coord, blacklistedCoordinates))
        continue;

      out.push_back(coord);
    }
  }
  return out;
}

std::vector<Coordinates> ComputerStrategyHelper::Cheat(
    const GameBoard& board, const std::vector<Coordinates>& blacklistedCoordinates
) {
  std::vector<Coordinates> out;
  const auto& s = board.GetSegmentBoard().Segments();
  const auto& units = board.Units();

  for (size_t y = 0; y < board.Height(); ++y) {
    for (size_t x = 0; x < board.Width(); ++x) {
      if (s[y][x])
        continue;

      if (units[y][x] == nullptr)
        continue;

      const Coordinates coord{x, y};

      if (ComputerStrategyHelper::IsCoordinateInList(coord, blacklistedCoordinates))
        continue;

      out.push_back(coord);
    }
  }

  return out;
}
