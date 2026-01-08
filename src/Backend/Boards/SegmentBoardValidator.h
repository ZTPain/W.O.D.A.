// SegmentBoardValidator.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "Backend/Games/GameMode.h"
#include "Backend/Units/BattleUnitType.h"
#include "ISegment.h"
#include <cstddef>
#include <unordered_map>
#include <vector>

class SegmentBoardValidator : public ISegment {
  ISegment& segmentBoard;
  const GameMode& mode;

public:
  SegmentBoardValidator(ISegment& segmentBoard, const GameMode& mode);
  [[nodiscard]] size_t Width() const override;
  [[nodiscard]] size_t Height() const override;
  [[nodiscard]] const std::vector<std::vector<bool>>& Segments() const override;
  bool ToggleSegment(size_t x, size_t y) override;
  void Clear() override;
  void GetUnits(
      std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>& outUnits
  ) const override;
};
