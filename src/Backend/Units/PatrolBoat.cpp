#include "PatrolBoat.h"
#include "BattleUnit.h"
#include "BattleUnitType.h"

PatrolBoat::PatrolBoat() {
  segments = 1;
  category = BattleUnitCategory::Marine;
}

BattleUnitType PatrolBoat::GetType() const { return BattleUnitType::PatrolBoat; }
