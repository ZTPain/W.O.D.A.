// Cruiser.h

#pragma once

#include "BattleUnit.h"

class Cruiser : public BattleUnit {
public:
  Cruiser();
  [[nodiscard]] BattleUnitType GetType() const override;
};
