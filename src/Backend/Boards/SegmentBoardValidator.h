// SegmentBoardValidator.h

#pragma once

#include "../Games/Coordinates.h"
#include "../Games/GameMode.h"
#include "../Units/BattleUnitType.h"
#include "ISegment.h"
#include "SegmentBoard.h"
#include <cstddef>
#include <unordered_map>
#include <vector>

class SegmentBoardValidator : public ISegment {
  SegmentBoard& segmentBoard;
  GameMode mode;

public:
  SegmentBoardValidator(size_t width, size_t height);
  [[nodiscard]] size_t Width() const override;
  [[nodiscard]] size_t Height() const override;
  [[nodiscard]] const std::vector<std::vector<bool>>& Segments() const override;
  bool ToggleSegment(size_t x, size_t y) override;
  void Clear() override;
  [[nodiscard]] const std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>&
  GetUnits() const override;
};
