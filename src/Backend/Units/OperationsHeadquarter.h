// OperationsHeadquarter.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include <array>
#include <vector>

class OperationsHeadquarter : public BattleUnit {
public:
  OperationsHeadquarter();
  static bool IsValidUnitShape(const std::vector<Coordinates>& segments);

  static inline const std::array<Coordinates, 4> EXPECTED_COORDINATES = {
      Coordinates(0, 0),
      Coordinates(1, 0),
      Coordinates(0, 1),
      Coordinates(1, 1),
  };
};
