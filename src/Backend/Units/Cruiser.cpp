#include "Cruiser.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

Cruiser::Cruiser() {
  type = BattleUnitType::Cruiser;
  category = BattleUnitCategory::Marine;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
