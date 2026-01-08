#include "PatrolBoat.h"
#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

PatrolBoat::PatrolBoat() {
  segments = BattleUnitHelper::GetSizeForUnitType(GetType());
  category = BattleUnitCategory::Marine;
}

BattleUnitType PatrolBoat::GetType() const { return BattleUnitType::PatrolBoat; }
