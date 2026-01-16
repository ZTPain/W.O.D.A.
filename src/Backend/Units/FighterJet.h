// FighterJet.h

#pragma once

#include "Backend/Games/Coordinates.h"
#include "BattleUnit.h"
#include <vector>

class FighterJet : public BattleUnit {
public:
  FighterJet();
  static bool IsValidUnitShape(const std::vector<Coordinates>& segments);
};
