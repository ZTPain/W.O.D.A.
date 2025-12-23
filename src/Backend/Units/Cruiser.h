// Cruiser.h

#pragma once

#include "BattleUnit.h"
#include "BattleUnitType.h"

class Cruiser : public BattleUnit {
public:
  Cruiser();
  [[nodiscard]] BattleUnitType GetType() const override;
};
