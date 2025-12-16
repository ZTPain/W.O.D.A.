// ISegment.h

#pragma once

#include "../Games/Coordinates.h"
#include "../Units/BattleUnitType.h"
#include <cstddef>
#include <unordered_map>
#include <vector>

class ISegment {
public:
  ISegment(size_t width, size_t height);
  virtual ~ISegment() = default;
  [[nodiscard]] virtual size_t Width() const = 0;
  [[nodiscard]] virtual size_t Height() const = 0;
  [[nodiscard]] virtual const std::vector<std::vector<bool>>& Segments() const = 0;
  virtual bool ToggleSegment(size_t x, size_t y) = 0;
  virtual void Clear() = 0;
  // Array[UnitType][UnitNumber] of Coordinates[]
  [[nodiscard]] virtual const std::
      unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>&
      GetUnits() const = 0;
};
