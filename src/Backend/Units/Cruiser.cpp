#include "Cruiser.h"
#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

Cruiser::Cruiser() {
  segments = BattleUnitHelper::GetSizeForUnitType(GetType());
  category = BattleUnitCategory::Marine;
}

BattleUnitType Cruiser::GetType() const { return BattleUnitType::Cruiser; }
