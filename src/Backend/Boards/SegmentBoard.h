// SegmentBoard.h

#pragma once

#include "../Games/Coordinates.h"
#include "../Units/BattleUnitType.h"
#include "ISegment.h"
#include <array>
#include <cstddef>
#include <unordered_map>
#include <vector>

template <size_t WIDTH, size_t HEIGHT> class SegmentBoard : public ISegment<WIDTH, HEIGHT> {
  std::array<std::array<bool, HEIGHT>, WIDTH> segments;

public:
  SegmentBoard();
  const std::array<std::array<bool, HEIGHT>, WIDTH>& Segments() const override;
  bool ToggleSegment(size_t x, size_t y) override;
  void Clear() override;
  [[nodiscard]] const std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>&
  GetUnits() const override;
};
