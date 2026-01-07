// ISegment.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "Backend/Units/BattleUnitType.h"
#include <cstddef>
#include <unordered_map>
#include <vector>

using UnitsMap = std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>;

class ISegment {
protected:
  size_t width;
  size_t height;
  std::vector<std::vector<bool>> segments;

public:
  virtual ~ISegment() = default;
  [[nodiscard]] size_t Width() const { return width; }
  [[nodiscard]] size_t Height() const { return height; }
  [[nodiscard]] const std::vector<std::vector<bool>>& Segments() const { return segments; }
  virtual bool ToggleSegment(size_t x, size_t y) = 0;
  virtual void Clear() = 0;
  // Array[UnitType][UnitNumber] of Coordinates[]
  [[nodiscard]] virtual const UnitsMap& GetUnits() const = 0;
};
