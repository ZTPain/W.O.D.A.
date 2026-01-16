// ArmoredTrain.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include <vector>

class ArmoredTrain : public BattleUnit {
public:
  ArmoredTrain();
  static bool IsValidUnitShape(const std::vector<Coordinates>& segments);
};
