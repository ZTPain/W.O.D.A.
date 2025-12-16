// GameBoard.h

#pragma once

#include "../Units/BattleUnit.h"
#include "SegmentBoard.h"
#include "SegmentBoardValidator.h"
#include "SegmentBoardView.h"
#include <array>
#include <cstddef>
#include <memory>

template <size_t WIDTH, size_t HEIGHT> class GameBoard {
  SegmentBoard<WIDTH, HEIGHT> segmentBoard;
  std::array<std::array<std::shared_ptr<BattleUnit>, HEIGHT>, WIDTH> units;

public:
  GameBoard();
  ~GameBoard();
  SegmentBoardValidator<WIDTH, HEIGHT> Segments();
  const std::array<std::array<std::shared_ptr<BattleUnit>, HEIGHT>, WIDTH>& Units() const;
  // Returns pointer to and dimensions of segment board
  [[nodiscard]] SegmentBoardView View() const;
  void ParseSegments();
  bool FireAt(size_t x, size_t y);
  void FixSegment(size_t x, size_t y);
  bool IsGameOver();
};
