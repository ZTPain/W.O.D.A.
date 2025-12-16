// Dreadnought.h

#pragma once

#include "BattleUnit.h"

class Dreadnought : public BattleUnit {
public:
  Dreadnought();
  [[nodiscard]] BattleUnitType GetType() const override;
};
