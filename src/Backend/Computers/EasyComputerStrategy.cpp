#include "EasyComputerStrategy.h"
#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include <cstddef>
#include <random>
#include <vector>

Coordinates EasyComputerStrategy::CalculateFireCoordinates(ISegment& board) const {
  const auto& s = board.Segments();
  size_t w = board.Width();
  size_t h = board.Height();

  std::vector<Coordinates> pool;

  for (size_t y = 0; y < h; ++y)
    for (size_t x = 0; x < w; ++x)
      if (!s[y][x])
        pool.emplace_back(x, y);

  static std::mt19937 rng{std::random_device{}()};
  std::uniform_int_distribution<size_t> dist(0, pool.size() - 1);
  return pool[dist(rng)];
}
