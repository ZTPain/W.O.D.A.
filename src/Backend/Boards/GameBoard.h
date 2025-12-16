// GameBoard.h

#pragma once

#include "../Units/BattleUnit.h"
#include "SegmentBoard.h"
#include "SegmentBoardValidator.h"
#include <cstddef>
#include <memory>
#include <vector>

class GameBoard {
  size_t width;
  size_t height;
  SegmentBoard segmentBoard;
  std::vector<std::vector<std::shared_ptr<BattleUnit>>> units;

public:
  GameBoard(size_t width, size_t height);
  ~GameBoard();
  [[nodiscard]] size_t Width() const;
  [[nodiscard]] size_t Height() const;
  SegmentBoardValidator SegmentBoard();
  [[nodiscard]] const std::vector<std::vector<std::shared_ptr<BattleUnit>>>& Units() const;
  void ParseSegments();
  bool FireAt(size_t x, size_t y);
  void FixSegment(size_t x, size_t y);
  bool IsGameOver();
};
