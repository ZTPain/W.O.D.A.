#include "MobileArtillery.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

MobileArtillery::MobileArtillery() {
  type = BattleUnitType::MobileArtillery;
  category = BattleUnitCategory::Land;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
