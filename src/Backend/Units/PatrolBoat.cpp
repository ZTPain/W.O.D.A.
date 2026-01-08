#include "PatrolBoat.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

PatrolBoat::PatrolBoat() {
  type = BattleUnitType::PatrolBoat;
  category = BattleUnitCategory::Marine;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
