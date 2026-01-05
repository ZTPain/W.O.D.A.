#include "Dreadnought.h"
#include "BattleUnit.h"
#include "BattleUnitType.h"

Dreadnought::Dreadnought() {
  segments = 4;
  category = BattleUnitCategory::Marine;
}

BattleUnitType Dreadnought::GetType() const { return BattleUnitType::Dreadnought; }
