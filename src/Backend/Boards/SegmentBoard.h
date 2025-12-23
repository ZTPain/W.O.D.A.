// SegmentBoard.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "Backend/Units/BattleUnitType.h"
#include "ISegment.h"
#include <cstddef>
#include <unordered_map>
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
  [[nodiscard]] const std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>&
  GetUnits() const override;
};
