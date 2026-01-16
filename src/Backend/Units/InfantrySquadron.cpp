#include "InfantrySquadron.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

InfantrySquadron::InfantrySquadron() {
  type = BattleUnitType::InfantrySquadron;
  category = BattleUnitCategory::Land;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
