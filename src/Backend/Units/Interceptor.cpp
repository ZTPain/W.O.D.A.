#include "Interceptor.h"
#include "BattleUnit.h"
#include "BattleUnitType.h"

Interceptor::Interceptor() {
  segments = 2;
  category = BattleUnitCategory::Marine;
}

BattleUnitType Interceptor::GetType() const { return BattleUnitType::Interceptor; }
