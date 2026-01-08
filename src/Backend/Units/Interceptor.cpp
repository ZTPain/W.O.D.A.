#include "Interceptor.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

Interceptor::Interceptor() {
  type = BattleUnitType::Interceptor;
  category = BattleUnitCategory::Marine;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
