// OperationsHeadquarter.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include <vector>

class OperationsHeadquarter : public BattleUnit {
public:
  OperationsHeadquarter();
  static bool IsValidUnitShape(const std::vector<Coordinates>& segments);
};
