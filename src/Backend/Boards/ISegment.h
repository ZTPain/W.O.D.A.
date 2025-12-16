// ISegment.h

#pragma once

#include "../Games/Coordinates.h"
#include "../Units/BattleUnitType.h"
#include <array>
#include <cstddef>
#include <unordered_map>
#include <vector>

template <size_t WIDTH, size_t HEIGHT> class ISegment {
public:
  virtual ~ISegment() = default;
  virtual bool ToggleSegment(size_t x, size_t y) = 0;
  [[nodiscard]] virtual const std::array<std::array<bool, HEIGHT>, WIDTH>& Segments() const = 0;
  virtual void Clear() = 0;
  // Array[UnitType][UnitNumber] of Coordinates[]
  [[nodiscard]] virtual const std::
      unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>&
      GetUnits() const = 0;
};
