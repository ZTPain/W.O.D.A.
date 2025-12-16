// Interceptor.h

#pragma once

#include "BattleUnit.h"

class Interceptor : public BattleUnit {
public:
  Interceptor();
  [[nodiscard]] BattleUnitType GetType() const override;
};
