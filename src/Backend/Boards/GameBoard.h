// GameBoard.h

#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Units/BattleUnit.h"
#include <cstddef>
#include <memory>
#include <vector>

class GameBoard {
  size_t width;
  size_t height;
  ISegment& segmentBoard;
  std::vector<std::vector<std::shared_ptr<BattleUnit>>> units;

public:
  GameBoard(size_t width, size_t height);
  ~GameBoard();
  [[nodiscard]] size_t Width() const;
  [[nodiscard]] size_t Height() const;
  ISegment SegmentBoard();
  [[nodiscard]] const std::vector<std::vector<std::shared_ptr<BattleUnit>>>& Units() const;
  void ParseSegments();
  bool FireAt(size_t x, size_t y);
  void FixSegment(size_t x, size_t y);
  bool IsGameOver();
};
