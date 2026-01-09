// SegmentBoard.h

#pragma once

#include "ISegment.h"
#include <cstddef>
#include <memory>
#include <vector>

class SegmentBoard : public ISegment {
  size_t width;
  size_t height;
  std::vector<std::vector<bool>> segments;

public:
  SegmentBoard(size_t width, size_t height);
  [[nodiscard]] size_t Width() const override;
  [[nodiscard]] size_t Height() const override;
  [[nodiscard]] const std::vector<std::vector<bool>>& Segments() const override;
  bool ToggleSegment(size_t x, size_t y) override;
  void Clear() override;
  [[nodiscard]] const UnitsMap& GetUnits() const override;
  [[nodiscard]] std::unique_ptr<ISegment> Clone() const override;
};
