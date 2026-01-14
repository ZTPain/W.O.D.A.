#include "MediumComputerStrategy.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include <cstddef>
#include <random>
#include <vector>

Coordinates MediumComputerStrategy::CalculateFireCoordinates(const GameBoard& board) const {
  const auto& s = board.GetSegmentBoard().Segments();
  const size_t w = board.Width();
  const size_t h = board.Height();

  std::vector<Coordinates> pool;

  for (size_t y = 0; y < h; ++y)
    for (size_t x = 0; x < w; ++x)
      if ((x + y) % 2 == 0 && !s[y][x])
        pool.emplace_back(x, y);

  if (pool.empty()) {
    for (size_t y = 0; y < h; ++y)
      for (size_t x = 0; x < w; ++x)
        if (!s[y][x])
          pool.emplace_back(x, y);
  }

  static std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<size_t> dist(0, pool.size() - 1);
  return pool[dist(rng)];
}
