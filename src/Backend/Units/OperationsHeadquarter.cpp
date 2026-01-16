#include "OperationsHeadquarter.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

OperationsHeadquarter::OperationsHeadquarter() {
  type = BattleUnitType::OperationsHeadquarter;
  category = BattleUnitCategory::Land;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
