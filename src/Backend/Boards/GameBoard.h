// GameBoard.h

#pragma once

#include "../Units/BattleUnit.h"
#include "SegmentBoard.h"
#include "SegmentBoardValidator.h"
#include <array>
#include <cstddef>
#include <memory>

template <size_t WIDTH, size_t HEIGHT> class GameBoard {
  SegmentBoard<WIDTH, HEIGHT> segments;
  std::array<std::array<std::shared_ptr<BattleUnit>, HEIGHT>, WIDTH> units;

public:
  GameBoard();
  ~GameBoard();
  SegmentBoardValidator<WIDTH, HEIGHT> Segments();
  const std::array<std::array<std::shared_ptr<BattleUnit>, HEIGHT>, WIDTH>& Units() const;
  void ParseSegments();
  bool FireAt(size_t x, size_t y);
  void FixSegment(size_t x, size_t y);
  bool IsGameOver();
};
