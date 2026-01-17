// SegmentBoardValidator.h

#pragma once

#include "Backend/Games/GameMode.h"
#include "ISegment.h"
#include <cstddef>
#include <vector>

class SegmentBoardValidator : public ISegment {
  ISegment& segmentBoard;
  const GameMode& mode;
  UnitsMap lastUnits;

public:
  SegmentBoardValidator(ISegment& segmentBoard, const GameMode& mode);
  [[nodiscard]] size_t Width() const override;
  [[nodiscard]] size_t Height() const override;
  [[nodiscard]] const std::vector<std::vector<bool>>& Segments() const override;
  [[nodiscard]] const std::vector<std::vector<bool>>& LandSegments() const override;
  bool ToggleSegment(size_t x, size_t y) override;
  void Clear() override;
  [[nodiscard]] const UnitsMap& GetUnits() const override;
};
