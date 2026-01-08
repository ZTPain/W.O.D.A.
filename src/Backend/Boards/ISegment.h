// ISegment.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "Backend/Units/BattleUnitType.h"
#include <cstddef>
#include <memory>
#include <unordered_map>
#include <vector>

using UnitsMap = std::unordered_map<BattleUnitType, std::vector<std::vector<Coordinates>>>;

class ISegment {
public:
  virtual ~ISegment() = default;
  [[nodiscard]] virtual size_t Width() const = 0;
  [[nodiscard]] virtual size_t Height() const = 0;
  [[nodiscard]] virtual const std::vector<std::vector<bool>>& Segments() const = 0;
  virtual bool ToggleSegment(size_t x, size_t y) = 0;
  virtual void Clear() = 0;
  // Array[UnitType][UnitNumber] of Coordinates[]
  [[nodiscard]] virtual const UnitsMap& GetUnits() const = 0;
  [[nodiscard]] virtual std::unique_ptr<ISegment> Clone() const = 0;
};
