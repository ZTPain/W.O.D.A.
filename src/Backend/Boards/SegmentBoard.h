// SegmentBoard.h

#pragma once

#include "ISegment.h"
#include <cstddef>
#include <vector>

class SegmentBoard : public ISegment {
public:
  SegmentBoard(size_t width, size_t height);
  bool ToggleSegment(size_t x, size_t y) override;
  void Clear() override;
  [[nodiscard]] const UnitsMap& GetUnits() const override;
};
