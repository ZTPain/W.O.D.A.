#include "Dreadnought.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

Dreadnought::Dreadnought() {
  type = BattleUnitType::Dreadnought;
  category = BattleUnitCategory::Marine;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
