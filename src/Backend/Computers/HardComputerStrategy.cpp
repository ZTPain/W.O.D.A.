#include "HardComputerStrategy.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Boards/ISegment.h"
#include "Backend/Games/Coordinates.h"
#include <cstddef>
#include <random>
#include <vector>

static std::mt19937& Rng() {
  static std::mt19937 rng{std::random_device{}()};
  return rng;
}

static Coordinates RandomFrom(const std::vector<Coordinates>& v) {
  std::uniform_int_distribution<size_t> d(0, v.size() - 1);
  return v[d(Rng())];
}

static std::vector<Coordinates> AllFree(const ISegment& board) {
  std::vector<Coordinates> out;
  const auto& s = board.Segments();

  for (size_t y = 0; y < board.Height(); ++y)
    for (size_t x = 0; x < board.Width(); ++x)
      if (!s[y][x])
        out.emplace_back(x, y);

  return out;
}

static std::vector<Coordinates> ChessboardFree(const ISegment& board) {
  std::vector<Coordinates> out;
  const auto& s = board.Segments();

  for (size_t y = 0; y < board.Height(); ++y)
    for (size_t x = 0; x < board.Width(); ++x)
      if ((x + y) % 2 == 0 && !s[y][x])
        out.emplace_back(x, y);

  return out;
}

static std::vector<Coordinates> CheatableTargets(const GameBoard& board) {
  std::vector<Coordinates> out;
  const auto& s = board.GetSegmentBoard().Segments();
  const auto& units = board.Units();

  for (size_t y = 0; y < board.Height(); ++y)
    for (size_t x = 0; x < board.Width(); ++x)
      if (!s[y][x] && units[y][x] != nullptr)
        out.emplace_back(x, y);

  return out;
}

static bool ShouldCheat() {
  static std::uniform_int_distribution<int> d(1, 100);
  return d(Rng()) <= 20;
}

Coordinates HardComputerStrategy::CalculateFireCoordinates(const GameBoard& board) const {
  if (ShouldCheat()) {
    const auto cheatTargets = CheatableTargets(board);
    if (!cheatTargets.empty())
      return RandomFrom(cheatTargets);
  }

  const auto chess = ChessboardFree(board.GetSegmentBoard());
  if (!chess.empty())
    return RandomFrom(chess);

  const auto all = AllFree(board.GetSegmentBoard());
  return RandomFrom(all);
}
