// GameBoard.h

#pragma once

#include "Backend/Boards/ISegment.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnit.h"
#include "SegmentBoardValidator.h"
#include <cstddef>
#include <memory>
#include <vector>

class GameBoard {
  const GameMode& mode;
  std::unique_ptr<ISegment> segmentBoard;
  std::unique_ptr<SegmentBoardValidator> segmentValidator;
  std::vector<std::vector<std::shared_ptr<BattleUnit>>> units;
  std::vector<std::shared_ptr<BattleUnit>> allUnits;

public:
  GameBoard(const GameMode& mode);
  ~GameBoard();
  [[nodiscard]] size_t Width() const;
  [[nodiscard]] size_t Height() const;
  ISegment& GetSegmentBoard();
  [[nodiscard]] const std::vector<std::vector<std::shared_ptr<BattleUnit>>>& Units() const;
  void ParseSegments();
  bool FireAt(size_t x, size_t y);
  void FixSegment(size_t x, size_t y);
  bool IsGameOver();
};
