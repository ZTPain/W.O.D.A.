#include "Cruiser.h"
#include "BattleUnit.h"
#include "BattleUnitType.h"

Cruiser::Cruiser() {
  segments = 3;
  category = BattleUnitCategory::Marine;
}

BattleUnitType Cruiser::GetType() const { return BattleUnitType::Cruiser; }
