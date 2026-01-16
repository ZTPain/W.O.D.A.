#include "GrenadeLauncher.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

GrenadeLauncher::GrenadeLauncher() {
  type = BattleUnitType::GrenadeLauncher;
  category = BattleUnitCategory::Land;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
