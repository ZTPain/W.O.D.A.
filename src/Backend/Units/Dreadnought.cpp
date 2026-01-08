#include "Dreadnought.h"
#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

Dreadnought::Dreadnought() {
  segments = BattleUnitHelper::GetSizeForUnitType(GetType());
  category = BattleUnitCategory::Marine;
}

BattleUnitType Dreadnought::GetType() const { return BattleUnitType::Dreadnought; }
