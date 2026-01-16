#include "FighterJet.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

FighterJet::FighterJet() {
  type = BattleUnitType::FighterJet;
  category = BattleUnitCategory::Aerial;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
