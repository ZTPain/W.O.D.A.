#include "HardComputerStrategy.h"
#include "Backend/Boards/GameBoard.h"
#include "Backend/Games/Coordinates.h"
#include <array>
#include <cstddef>
#include <random>
#include <utility>
#include <vector>

static std::mt19937& Rng() {
  static std::mt19937 rng{std::random_device{}()};
  return rng;
}

static Coordinates RandomFrom(const std::vector<Coordinates>& v) {
  std::uniform_int_distribution<size_t> d(0, v.size() - 1);
  return v[d(Rng())];
}

static bool InBounds(int x, int y, size_t w, size_t h) {
  return x >= 0 && y >= 0 && x < static_cast<int>(w) && y < static_cast<int>(h);
}

static std::vector<Coordinates> Neighbors(size_t x, size_t y, size_t w, size_t h) {
  const std::array<std::pair<int, int>, 4> dirs{
      {{1, 0}, {-1, 0}, {0, 1}, {0, -1}}
  };
  std::vector<Coordinates> out;
  for (auto [dx, dy] : dirs) {
    const int nx = static_cast<int>(x) + dx;
    const int ny = static_cast<int>(y) + dy;
    if (InBounds(nx, ny, w, h))
      out.emplace_back(static_cast<size_t>(nx), static_cast<size_t>(ny));
  }
  return out;
}

static std::vector<Coordinates> FinishDamagedShipsWeighted(const GameBoard& board) {
  std::vector<Coordinates> weightedPool;
  const auto& shots = board.GetSegmentBoard().Segments();
  const auto& units = board.Units();
  const size_t w = board.Width();
  const size_t h = board.Height();

  for (size_t y = 0; y < h; ++y) {
    for (size_t x = 0; x < w; ++x) {
      auto u = units.at(y).at(x);
      if (!u)
        continue;
      if (!shots.at(y).at(x) || u->IsDestroyed())
        continue;

      for (auto [nx, ny] : Neighbors(x, y, w, h)) {
        if (shots.at(ny).at(nx))
          continue;
        if (units.at(ny).at(nx)) {
          weightedPool.emplace_back(nx, ny);
          weightedPool.emplace_back(nx, ny);
          weightedPool.emplace_back(nx, ny);
        } else {
          weightedPool.emplace_back(nx, ny);
        }
      }
    }
  }

  return weightedPool;
}

static std::vector<Coordinates> AllFree(const GameBoard& board) {
  std::vector<Coordinates> out;
  const auto& s = board.GetSegmentBoard().Segments();
  for (size_t y = 0; y < board.Height(); ++y)
    for (size_t x = 0; x < board.Width(); ++x)
      if (!s.at(y).at(x))
        out.emplace_back(x, y);
  return out;
}

static std::vector<Coordinates> ChessboardFree(const GameBoard& board) {
  std::vector<Coordinates> out;
  const auto& s = board.GetSegmentBoard().Segments();
  for (size_t y = 0; y < board.Height(); ++y)
    for (size_t x = 0; x < board.Width(); ++x)
      if ((x + y) % 2 == 0 && !s.at(y).at(x))
        out.emplace_back(x, y);
  return out;
}

static std::vector<Coordinates> CheatableTargets(const GameBoard& board) {
  std::vector<Coordinates> out;
  const auto& s = board.GetSegmentBoard().Segments();
  const auto& units = board.Units();

  for (size_t y = 0; y < board.Height(); ++y)
    for (size_t x = 0; x < board.Width(); ++x)
      if (!s.at(y).at(x) && units.at(y).at(x) != nullptr)
        out.emplace_back(x, y);

  return out;
}

static bool ShouldCheat() {
  std::uniform_int_distribution<int> d(1, 100);
  return d(Rng()) <= 20;
}

Coordinates HardComputerStrategy::CalculateFireCoordinates(const GameBoard& board) const {

  auto finishTargets = FinishDamagedShipsWeighted(board);
  if (!finishTargets.empty())
    return RandomFrom(finishTargets);

  if (ShouldCheat()) {
    auto cheatTargets = CheatableTargets(board);
    if (!cheatTargets.empty())
      return RandomFrom(cheatTargets);
  }

  auto chess = ChessboardFree(board);
  if (!chess.empty())
    return RandomFrom(chess);

  auto all = AllFree(board);
  return RandomFrom(all);
}
