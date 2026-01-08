#include "Interceptor.h"
#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

Interceptor::Interceptor() {
  segments = BattleUnitHelper::GetSizeForUnitType(GetType());
  category = BattleUnitCategory::Marine;
}

BattleUnitType Interceptor::GetType() const { return BattleUnitType::Interceptor; }
