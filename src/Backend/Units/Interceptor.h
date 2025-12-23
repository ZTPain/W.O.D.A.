// Interceptor.h

#pragma once

#include "BattleUnit.h"
#include "BattleUnitType.h"

class Interceptor : public BattleUnit {
public:
  Interceptor();
  [[nodiscard]] BattleUnitType GetType() const override;
};
