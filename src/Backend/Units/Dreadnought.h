// Dreadnought.h

#pragma once

#include "BattleUnit.h"
#include "BattleUnitType.h"

class Dreadnought : public BattleUnit {
public:
  Dreadnought();
  [[nodiscard]] BattleUnitType GetType() const override;
};
