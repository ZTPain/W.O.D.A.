#include "ArmoredTrain.h"

#include "BattleUnit.h"
#include "BattleUnitHelper.h"
#include "BattleUnitType.h"

ArmoredTrain::ArmoredTrain() {
  type = BattleUnitType::ArmoredTrain;
  category = BattleUnitCategory::Land;
  segments = BattleUnitHelper::GetSizeForUnitType(type);
}
